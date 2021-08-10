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

std::shared_ptr<Expression> EVOParser::parse_member_expression()
{
    auto lhs = parse_primary_expression();
    if(lhs->is_error())
        return lhs;

    auto dot = consume_of_type(Token::Dot);
    if(!dot)
        return lhs;

    auto member_name = consume_of_type(Token::Name);
    if(!member_name)
        return std::make_shared<MemberExpression>(ASTNode::Error, "Expected name in member expression");

    return std::make_shared<MemberExpression>(lhs, member_name->value());
}

std::shared_ptr<Expression> EVOParser::parse_assignment_expression()
{
    auto lhs = parse_member_expression();
    if(lhs->is_error())
        return lhs;

    auto op = consume_of_type(Token::AssignmentOperator);
    if(!op)
        return lhs;

    auto rhs = parse_assignment_expression();
    if(rhs->is_error())
        return rhs;
    return std::make_shared<AssignmentExpression>(lhs, rhs);
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

    auto semicolon = consume_of_type(Token::Semicolon);
    if(!semicolon)
        return std::make_shared<ExpressionStatement>(ASTNode::Error, "Expected ';' in statement");
    
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
    }
    return program;
}

}
