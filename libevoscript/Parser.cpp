#include <libevoscript/AST.h>
#include <libevoscript/Parser.h>

#include <iostream>

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

// ( expression ) | integer_literal | string_literal | identifier | special_value
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
            literal = parse_string_literal();
            if(literal->is_error())
            {
                set_offset(off);
                literal = parse_special_value();
                if(literal->is_error())
                {
                    set_offset(off);
                    literal = parse_identifier();
                    if(literal->is_error())
                        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Expected primary expression"));
                }
            }
        }
        return literal;
    }

    auto expression = parse_expression();
    if(expression->is_error())
        return expression;
    
    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<Expression>(ASTNode::Error(location(), "Unmatched '('"));

    return expression;
}

std::shared_ptr<Expression> EVOParser::parse_integer_literal()
{
    auto name = consume_of_type(Token::Number);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid literal"));

    try
    {
        return std::make_shared<IntegerLiteral>(std::stoi(name->value()));
    }
    catch(...)
    {
        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid integer literal"));
    }
}

std::shared_ptr<Expression> EVOParser::parse_string_literal()
{
    auto name = consume_of_type(Token::String);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid literal"));

    return std::make_shared<StringLiteral>(name->value());
}

std::shared_ptr<Expression> EVOParser::parse_identifier()
{
    auto name = consume_of_type(Token::Name);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid identifier"));

    return std::make_shared<Identifier>(name->value());
}

std::shared_ptr<Expression> EVOParser::parse_special_value()
{
    auto name = consume_of_type(Token::Name);
    if(!name)
        return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid special value"));

    if(name->value() == "this")
        return std::make_shared<SpecialValue>(SpecialValue::This);
    if(name->value() == "global")
        return std::make_shared<SpecialValue>(SpecialValue::Global);
    if(name->value() == "null")
        return std::make_shared<SpecialValue>(SpecialValue::Null);
    if(name->value() == "true")
        return std::make_shared<SpecialValue>(SpecialValue::True);
    if(name->value() == "false")
        return std::make_shared<SpecialValue>(SpecialValue::False);
    if(name->value() == "undefined")
        return std::make_shared<SpecialValue>(SpecialValue::Undefined);

    return std::make_shared<SpecialValue>(ASTNode::Error(location(), "Invalid special value"));
}

std::shared_ptr<Expression> EVOParser::parse_member_name(std::shared_ptr<Expression> lhs)
{
    auto dot = consume_of_type(Token::Dot);
    if(!dot)
        return {};

    auto member_name = consume_of_type(Token::Name);
    if(!member_name)
        return std::make_shared<MemberExpression>(ASTNode::Error(location(), "Expected name in member expression"));

    return std::make_shared<MemberExpression>(lhs, member_name->value());
}

std::shared_ptr<Expression> EVOParser::parse_argument_list(std::shared_ptr<Expression> lhs)
{
    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return {};

    std::vector<std::shared_ptr<Expression>> arguments;

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
    {
        while(true)
        {
            auto argument = parse_expression();
            if(argument->is_error())
                return argument;

            arguments.push_back(argument);

            auto comma = consume_of_type(Token::Comma);
            if(!comma)
                break;
        }
        
        auto paren_close = consume_of_type(Token::ParenClose);
        if(!paren_close)
            return std::make_shared<FunctionCall>(ASTNode::Error(location(), "Unmatched '('"));
    }

    return std::make_shared<FunctionCall>(lhs, arguments);
}

std::shared_ptr<Expression> EVOParser::parse_postfix_operator(std::shared_ptr<Expression> lhs)
{
    auto op = consume_of_type(Token::NormalOperator);
    if(!op)
        return {};
    
    UnaryExpression::Operation operation;
    if(op->value() == "++")
        operation = UnaryExpression::Operation::PostfixIncrement;
    else if(op->value() == "--")
        operation = UnaryExpression::Operation::PostfixDecrement;
    else
        return {};

    return std::make_shared<UnaryExpression>(lhs, operation);
}

std::shared_ptr<Expression> EVOParser::parse_postfix_expression()
{
    auto lhs = parse_primary_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        std::shared_ptr<Expression> new_lhs = parse_member_name(lhs);
        if(!new_lhs)
        {
            set_offset(off);
            new_lhs = parse_argument_list(lhs);
            if(!new_lhs)
            {
                set_offset(off);
                new_lhs = parse_postfix_operator(lhs);
                if(!new_lhs)
                {
                    set_offset(off);
                    return lhs;
                }
            }
        }
        if(new_lhs->is_error())
            return new_lhs;
        lhs = new_lhs;
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_unary_expression()
{
    auto op = consume_of_type(Token::NormalOperator);
    if(!op)
        return parse_postfix_expression();

    auto lhs = parse_unary_expression();
    if(lhs->is_error())
        return lhs;

    UnaryExpression::Operation operation;
    if(op->value() == "!")
        operation = UnaryExpression::Not;
    else if(op->value() == "~")
        operation = UnaryExpression::BitwiseNot;
    else if(op->value() == "-")
        operation = UnaryExpression::Minus;
    else if(op->value() == "+")
        operation = UnaryExpression::Plus;
    else if(op->value() == "++")
        operation = UnaryExpression::Increment;
    else if(op->value() == "--")
        operation = UnaryExpression::Decrement;
    else
        return std::make_shared<UnaryExpression>(ASTNode::Error(location(), "Invalid unary expression"));
    
    return std::make_shared<UnaryExpression>(lhs, operation);
}

std::shared_ptr<Expression> EVOParser::parse_multiplicative_expression()
{
    auto lhs = parse_unary_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        auto op = consume_of_type(Token::NormalOperator);
        if(!op)
            return lhs;

        auto rhs = parse_unary_expression();
        if(rhs->is_error())
        {
            set_offset(off);
            return rhs;
        }

        NormalBinaryExpression::Operation operation;
        if(op->value() == "*")
            operation = NormalBinaryExpression::Multiply;
        else if(op->value() == "/")
            operation = NormalBinaryExpression::Divide;
        else if(op->value() == "%")
            operation = NormalBinaryExpression::Modulo;
        else
        {
            set_offset(off);
            return lhs;
        }

        lhs = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_additive_expression()
{
    auto lhs = parse_multiplicative_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        auto op = consume_of_type(Token::NormalOperator);
        if(!op)
            return lhs;

        auto rhs = parse_multiplicative_expression();
        if(rhs->is_error())
        {
            set_offset(off);
            return rhs;
        }

        NormalBinaryExpression::Operation operation;
        if(op->value() == "+")
            operation = NormalBinaryExpression::Add;
        else if(op->value() == "-")
            operation = NormalBinaryExpression::Subtract;
        else
        {
            set_offset(off);
            return lhs;
        }

        lhs = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_comparison_expression()
{
    auto lhs = parse_additive_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        auto op = consume_of_type(Token::NormalOperator);
        if(!op)
            return lhs;

        auto rhs = parse_additive_expression();
        if(rhs->is_error())
        {
            set_offset(off);
            return rhs;
        }

        NormalBinaryExpression::Operation operation;
        if(op->value() == "==")
            operation = NormalBinaryExpression::Equal;
        else if(op->value() == "!=")
            operation = NormalBinaryExpression::NonEqual;
        else if(op->value() == ">")
            operation = NormalBinaryExpression::Greater;
        else if(op->value() == ">=")
            operation = NormalBinaryExpression::GreaterEqual;
        else if(op->value() == "<")
            operation = NormalBinaryExpression::Less;
        else if(op->value() == "<=")
            operation = NormalBinaryExpression::LessEqual;
        else
        {
            set_offset(off);
            return lhs;
        }

        lhs = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_logical_and_expression()
{
    auto lhs = parse_comparison_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        auto op = consume_of_type(Token::NormalOperator);
        if(!op)
            return lhs;

        auto rhs = parse_comparison_expression();
        if(rhs->is_error())
        {
            set_offset(off);
            return rhs;
        }

        NormalBinaryExpression::Operation operation;
        if(op->value() == "&&")
            operation = NormalBinaryExpression::And;
        else
        {
            set_offset(off);
            return lhs;
        }

        lhs = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_logical_or_expression()
{
    auto lhs = parse_logical_and_expression();
    if(lhs->is_error())
        return lhs;

    while(true)
    {
        size_t off = offset();
        auto op = consume_of_type(Token::NormalOperator);
        if(!op)
            return lhs;

        auto rhs = parse_logical_and_expression();
        if(rhs->is_error())
        {
            set_offset(off);
            return rhs;
        }

        NormalBinaryExpression::Operation operation;
        if(op->value() == "||")
            operation = NormalBinaryExpression::Or;
        else
        {
            set_offset(off);
            return lhs;
        }

        lhs = std::make_shared<NormalBinaryExpression>(lhs, rhs, operation);
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_assignment_expression()
{
    auto lhs = parse_logical_or_expression();
    if(lhs->is_error())
        return lhs;

    auto op = consume_of_type(Token::AssignmentOperator);
    if(!op)
        return lhs;

    auto rhs = parse_assignment_expression();
    if(rhs->is_error())
        return rhs;

    AssignmentExpression::Operation operation;
    if(op->value() == "=")
        operation = AssignmentExpression::Assign;
    else if(op->value() == "+=")
        operation = AssignmentExpression::Add;
    else if(op->value() == "-=")
        operation = AssignmentExpression::Subtract;
    else if(op->value() == "*=")
        operation = AssignmentExpression::Multiply;
    else if(op->value() == "/=")
        operation = AssignmentExpression::Divide;
    else if(op->value() == "%=")
        operation = AssignmentExpression::Modulo;
    else
        assert(false);

    return std::make_shared<AssignmentExpression>(lhs, rhs, operation);
}

std::shared_ptr<Declaration> EVOParser::parse_declaration()
{
    size_t off = offset();
    std::shared_ptr<Declaration> declaration = parse_variable_declaration();
    if(!declaration)
    {
        set_offset(off);
        return {};
    }
    return declaration;
}

std::shared_ptr<VariableDeclaration> EVOParser::parse_variable_declaration()
{
    auto let = consume_of_type(Token::Name);
    if(!let || let->value() != "let")
        return {};
    
    auto name = consume_of_type(Token::Name);
    if(!name)
        return std::make_shared<VariableDeclaration>(ASTNode::Error(location(), "Expected variable name"));
    
    auto equal = consume_of_type(Token::AssignmentOperator);
    if(!equal)
        return std::make_shared<VariableDeclaration>(name->value(), nullptr);
    if(equal->value() != "=")
        // TODO: Fix syntax error displayed at expression instead of operator itself
        return std::make_shared<VariableDeclaration>(ASTNode::Error(location(), "Invalid operator for initializer, expected '='"));

    auto initializer = parse_expression();
    if(initializer->is_error())
        return std::make_shared<VariableDeclaration>(initializer->errors());
    
    return std::make_shared<VariableDeclaration>(name->value(), initializer);
}

std::shared_ptr<Statement> EVOParser::parse_expression_statement()
{
    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<ExpressionStatement>(expression->errors());
    
    return std::make_shared<ExpressionStatement>(expression);
}

std::shared_ptr<Statement> EVOParser::parse_block_statement()
{
    auto curly_open = consume_of_type(Token::CurlyOpen);
    if(!curly_open)
        return {};

    auto statement = std::make_shared<BlockStatement>();
    while(!eof())
    {
        auto curly_close = consume_of_type(Token::CurlyClose);
        if(curly_close)
            return statement;

        auto node = parse_statement();
        if(!node)
            break;
        statement->add_node(node);
        if(node->is_error())
            break;

        if(node->requires_semicolon())
        {
            auto semicolon = consume_of_type(Token::Semicolon);
            if(!semicolon)
            {
                auto curly_close = consume_of_type(Token::CurlyClose);
                if(curly_close)
                    return statement;
                return std::make_shared<BlockStatement>(ASTNode::Error(location(), "Expected ';' after statement"));
            }
        }
    }

    auto curly_close = consume_of_type(Token::CurlyClose);
    if(!curly_close)
        return std::make_shared<BlockStatement>(ASTNode::Error(location(), "Unclosed block statement"));

    return statement;
}

std::shared_ptr<Statement> EVOParser::parse_if_statement()
{
    auto if_keyword = consume_of_type(Token::Name);
    if(!if_keyword || if_keyword->value() != "if")
        return {};

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected '(' after 'if'"));

    auto condition = parse_expression();
    if(condition->is_error())
        return std::make_shared<ExpressionStatement>(condition->errors());

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected ')' after 'if' condition"));

    auto true_statement = parse_statement();
    if(!true_statement || true_statement->is_error())
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected statement"));

    return std::make_shared<IfStatement>(condition, true_statement);
}

std::shared_ptr<Statement> EVOParser::parse_statement()
{
    size_t off = offset();
    auto statement = parse_block_statement();
    if(!statement)
    {
        set_offset(off);
        statement = parse_if_statement();
        if(!statement)
        {
            set_offset(off);
            statement = parse_declaration();
            if(!statement)
            {
                set_offset(off);
                statement = parse_expression_statement();
                if(!statement || statement->is_error())
                {
                    set_offset(off);
                    return statement;
                }
            }
        }
    }
    assert(statement);
    return statement;
}

std::shared_ptr<Program> EVOParser::parse_program()
{
    if(eof())
        return std::make_shared<Program>(ASTNode::Error(location(), "Empty program"));

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
                // TODO: Fix error propagation here.
                return std::make_shared<Program>(ASTNode::Error(location(), "Expected ';' after statement"));
        }
    }
    return program;
}

}
