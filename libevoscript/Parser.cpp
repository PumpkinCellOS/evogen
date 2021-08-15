#include "libevoscript/AST.h"
#include <libevoscript/Parser.h>

namespace evo::script
{

std::ostream& operator<<(std::ostream& stream, SourceLocation const& location)
{
    return stream << location.line << ":" << location.column;
}

std::shared_ptr<Expression> EVOParser::parse_expression()
{
    return parse_assignment_expression();
}

// ( expression ) | integer_literal | identifier | special_value
std::shared_ptr<Expression> EVOParser::parse_primary_expression()
{
    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
    {
        size_t off = offset();
        auto literal = parse_integer_literal();
        if(literal->is_error())
        {
            set_offset(off);
            literal = parse_identifier();
            if(literal->is_error())
            {
                set_offset(off);
                return parse_special_value();
            }
        }
        return literal;
    }

    auto expression = parse_expression();
    if(expression->is_error())
        return expression;
    
    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<Expression>(ASTNode::Error, "Unmatched '('");

    return expression;
}

std::shared_ptr<Expression> EVOParser::parse_integer_literal()
{
    auto name = consume_of_type(Token::Number);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error, "Invalid identifier");

    try
    {
        return std::make_shared<IntegerLiteral>(std::stoi(name->value()));
    }
    catch(...)
    {
        return std::make_shared<SpecialValue>(ASTNode::Error, "Invalid integer literal");
    }
}

std::shared_ptr<Expression> EVOParser::parse_identifier()
{
    auto name = consume_of_type(Token::Name);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error, "Invalid identifier");

    return std::make_shared<Identifier>(name->value());
}

std::shared_ptr<Expression> EVOParser::parse_special_value()
{
    auto name = consume_of_type(Token::ReservedKeyword);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error, "Invalid special value");

    if(name->value() == "this")
        return std::make_shared<SpecialValue>(SpecialValue::This);
    if(name->value() == "null")
        return std::make_shared<SpecialValue>(SpecialValue::Null);
    if(name->value() == "undefined")
        return std::make_shared<SpecialValue>(SpecialValue::Undefined);

    return std::make_shared<SpecialValue>(ASTNode::Error, "Invalid special value");
}

std::shared_ptr<Expression> EVOParser::parse_member_name(std::shared_ptr<Expression> container)
{
    auto dot = consume_of_type(Token::Dot);
    if(!dot)
        return container;

    auto member_name = consume_of_type(Token::Name);
    if(!member_name)
        return std::make_shared<MemberExpression>(ASTNode::Error, "Expected name in member expression");

    auto member_expression = std::make_shared<MemberExpression>(container, member_name->value());

    auto maybe_chained_reference = parse_member_name(member_expression);
    if(!maybe_chained_reference->is_error())
        return maybe_chained_reference;
    return member_expression;
}

std::shared_ptr<Expression> EVOParser::parse_member_expression()
{
    auto lhs = parse_primary_expression();
    if(lhs->is_error())
        return lhs;

    return parse_member_name(lhs);
}

std::shared_ptr<Expression> EVOParser::parse_argument_list(std::shared_ptr<Expression> callable)
{
    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<FunctionCall>(ASTNode::Error, "Invalid argument list");

    // TODO: Arguments
    
    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<FunctionCall>(ASTNode::Error, "Unmatched '('");

    auto function_call = std::make_shared<FunctionCall>(callable);

    auto maybe_chained_call = parse_argument_list(function_call);
    if(!maybe_chained_call->is_error())
        return maybe_chained_call;
    return function_call;
}

std::shared_ptr<Expression> EVOParser::parse_function_call()
{
    auto lhs = parse_member_expression();
    if(lhs->is_error())
        return lhs;

    auto function_call = parse_argument_list(lhs);
    if(function_call->is_error())
        return lhs;
    return function_call;
}

std::shared_ptr<Expression> EVOParser::parse_additive_expression(std::shared_ptr<Expression> lhs)
{
    if(!lhs)
    {
        lhs = parse_function_call();
        if(lhs->is_error())
            return lhs;
    }

    auto op = consume_of_type(Token::NormalOperator);
    if(!op)
        return lhs;

    auto rhs = parse_function_call();
    if(rhs->is_error())
        return rhs;

    NormalBinaryExpression::Operation operation;
    if(op->value() == "+")
        operation = NormalBinaryExpression::Add;
    else if(op->value() == "-")
        operation = NormalBinaryExpression::Subtract;
    else
        assert(false);

    auto expression = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    auto maybe_additive_expression = parse_additive_expression(expression);

    return !maybe_additive_expression->is_error() ? maybe_additive_expression : expression;
}

std::shared_ptr<Expression> EVOParser::parse_assignment_expression()
{
    auto lhs = parse_additive_expression(nullptr);
    if(lhs->is_error())
        return lhs;

    auto op = consume_of_type(Token::AssignmentOperator);
    if(!op)
        return lhs;

    auto rhs = parse_assignment_expression();
    if(rhs->is_error())
        return rhs;
    return std::make_shared<AssignmentExpression>(lhs, rhs, AssignmentExpression::Assign);
}

std::shared_ptr<Statement> EVOParser::parse_statement()
{
    return parse_expression_statement();
}

std::shared_ptr<Statement> EVOParser::parse_expression_statement()
{
    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<ExpressionStatement>(ASTNode::Error, expression->error_message());
    
    return std::make_shared<ExpressionStatement>(expression);
}

std::shared_ptr<Program> EVOParser::parse_program()
{
    if(eof())
        return std::make_shared<Program>(ASTNode::Error, "Empty program");

    auto program = std::make_shared<Program>();
    while(!eof())
    {
        auto node = parse_statement();

        program->add_node(node);
        if(node->is_error())
            break;

        if(node->requires_semicolon())
        {
            auto semicolon = consume_of_type(Token::Semicolon);
            if(!semicolon && !eof())
                return std::make_shared<Program>(ASTNode::Error, "Expected ';' in statement");
        }
    }
    return program;
}

}
