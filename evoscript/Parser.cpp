#include <evoscript/AST.h>
#include <evoscript/Parser.h>

#include <iostream>
#include <variant>

namespace evo::script
{

std::shared_ptr<Expression> EVOParser::parse_expression()
{
    return parse_assignment_expression();
}

// ( expression ) | function_expression | integer_literal | string_literal | identifier | special_value
std::shared_ptr<Expression> EVOParser::parse_primary_expression()
{
    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
    {
        size_t off = offset();
        std::shared_ptr<Expression> literal = parse_function_expression();
        if(!literal)
        {
            set_offset(off);
            literal = parse_integer_literal();
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

std::shared_ptr<FunctionExpression> EVOParser::parse_function_expression()
{
    auto function_keyword = consume_of_type(Token::Name);
    if(!function_keyword || function_keyword->value() != "function")
        return {};

    auto name = consume_of_type(Token::Name);
    std::string function_name;
    if(name)
        function_name = name->value();

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<FunctionExpression>(ASTNode::Error(location(), "Expected '('"));

    // Arguments / formal parameters
    std::vector<StringId> arg_names;

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
    {
        while(true)
        {
            // TODO: Default values
            auto name = consume_of_type(Token::Name);
            if(!name)
                return std::make_shared<FunctionExpression>(ASTNode::Error(location(), "Expected argument name"));
            arg_names.push_back(name->value());

            auto comma = consume_of_type(Token::Comma);
            if(!comma)
                break;
        }
        
        paren_close = consume_of_type(Token::ParenClose);
        if(!paren_close)
            return std::make_shared<FunctionExpression>(ASTNode::Error(location(), "Unmatched '('"));
    }

    auto body = parse_block_statement();
    if(!body)
        return std::make_shared<FunctionExpression>(ASTNode::Error(location(), "Expected function body"));
    if(body->ASTGroupNode::is_error())
        return std::make_shared<FunctionExpression>(body->ASTGroupNode::errors());

    return std::make_shared<FunctionExpression>(function_name, body, arg_names);
}

std::shared_ptr<IntegerLiteral> EVOParser::parse_integer_literal()
{
    auto name = consume_of_type(Token::Number);
    if(!name)
        return std::make_shared<IntegerLiteral>(ASTNode::Error(location(), "Invalid literal"));

    try
    {
        if(name->value().starts_with("0x"))
            return std::make_shared<IntegerLiteral>(std::stoll(name->value().substr(2), nullptr, 16));
        return std::make_shared<IntegerLiteral>(std::stoll(name->value()));
    }
    catch(...)
    {
        return std::make_shared<IntegerLiteral>(ASTNode::Error(location(), "Invalid integer literal"));
    }
}

std::shared_ptr<StringLiteral> EVOParser::parse_string_literal()
{
    auto name = consume_of_type(Token::String);
    if(!name)
        return std::make_shared<StringLiteral>(ASTNode::Error(location(), "Invalid literal"));

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

std::shared_ptr<Expression> EVOParser::parse_subscript_value(std::shared_ptr<Expression> lhs)
{
    auto paren_open = consume_of_type(Token::BraceOpen);
    if(!paren_open)
        return {};

    auto value = parse_expression();

    auto paren_close = consume_of_type(Token::BraceClose);
    if(!paren_close)
        return std::make_shared<Subscript>(ASTNode::Error(location(), "Unmatched '['"));

    return value->is_error() ? value : std::make_shared<Subscript>(lhs, value);
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
                new_lhs = parse_subscript_value(lhs);
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
        }
        if(new_lhs->is_error())
            return new_lhs;
        lhs = new_lhs;
    }
    assert(false);
}

std::shared_ptr<Expression> EVOParser::parse_new_expression()
{
    auto new_keyword = consume_of_type(Token::Name);
    if(!new_keyword || new_keyword->value() != "new")
        return {};

    // 'primary' because of ambiguity with function calls
    auto name = parse_primary_expression();
    if(name->is_error())
        return name;

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<NewExpression>(ASTNode::Error(location(), "Expected '('"));

    // FIXME: Code dupe with parse_function_call??
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
            return std::make_shared<NewExpression>(ASTNode::Error(location(), "Unmatched '('"));
    }

    return std::make_shared<NewExpression>(name, arguments);
}

std::shared_ptr<Expression> EVOParser::parse_unary_expression()
{
    {
        size_t off = offset();
        auto new_expression = parse_new_expression();
        if(new_expression)
            return new_expression;
        set_offset(off);
    }

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
    {
        assert(false);
        return {};
    }

    return std::make_shared<AssignmentExpression>(lhs, rhs, operation);
}

std::shared_ptr<Declaration> EVOParser::parse_declaration()
{
    size_t off = offset();
    std::shared_ptr<Declaration> declaration = parse_variable_declaration();
    if(!declaration)
    {
        set_offset(off);
        declaration = parse_function_declaration();
        if(!declaration)
        {
            set_offset(off);
            return {};
        }
    }
    return declaration;
}

std::shared_ptr<VariableDeclaration> EVOParser::parse_variable_declaration()
{
    auto let = consume_of_type(Token::Name);
    if(!let || (let->value() != "let" && let->value() != "const"))
        return {};
    
    auto name = consume_of_type(Token::Name);
    if(!name)
        return std::make_shared<VariableDeclaration>(ASTNode::Error(location(), "Expected variable name"));

    VariableDeclaration::Type type = let->value() == "const" ? VariableDeclaration::Const : VariableDeclaration::Let;
    
    auto equal = consume_of_type(Token::AssignmentOperator);
    if(!equal)
        return std::make_shared<VariableDeclaration>(type, name->value(), nullptr);
    if(equal->value() != "=")
        // TODO: Fix syntax error displayed at expression instead of operator itself
        return std::make_shared<VariableDeclaration>(ASTNode::Error(location(), "Invalid operator for initializer, expected '='"));

    auto initializer = parse_expression();
    if(initializer->is_error())
        return std::make_shared<VariableDeclaration>(initializer->errors());
    
    if(m_current_block)
        m_current_block->set_needs_scope();
    return std::make_shared<VariableDeclaration>(type, name->value(), initializer);
}

std::shared_ptr<FunctionDeclaration> EVOParser::parse_function_declaration()
{
    std::shared_ptr<FunctionExpression> expression = parse_function_expression();
    if(!expression)
        return {};
    if(expression->is_error())
        return std::make_shared<FunctionDeclaration>(expression->errors());
    if(m_current_block)
        m_current_block->set_needs_scope();
    return std::make_shared<FunctionDeclaration>(expression);
}

std::shared_ptr<Statement> EVOParser::parse_expression_statement()
{
    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<ExpressionStatement>(expression->errors());
    
    return std::make_shared<ExpressionStatement>(expression);
}

EVOParser::Label EVOParser::parse_label()
{
    size_t off = offset();
    auto case_or_default = consume_of_type(Token::Name);
    if(!case_or_default)
        return NoCaseLabel::None;
    if(case_or_default->value() == "case")
    {
        // TODO: Other literals
        auto literal = parse_integer_literal();
        if(!literal)
            return std::make_shared<CaseLabel>(ASTNode::Error(location(), "Expected literal in case label"));
        auto colon = consume_of_type(Token::Colon);
        if(!colon)
            return std::make_shared<CaseLabel>(ASTNode::Error(location(), "Expected ':'"));
        return std::make_shared<CaseLabel>(literal);
    }
    else if(case_or_default->value() == "default")
    {
        auto colon = consume_of_type(Token::Colon);
        if(!colon)
            return std::make_shared<CaseLabel>(ASTNode::Error(location(), "Expected ':'"));
        return NoCaseLabel::Default;
    }
    set_offset(off);
    return NoCaseLabel::None;
}

std::shared_ptr<BlockStatement> EVOParser::parse_block_statement()
{
    auto curly_open = consume_of_type(Token::CurlyOpen);
    if(!curly_open)
        return {};

    auto statement = std::make_shared<BlockStatement>();
    m_current_block = statement.get();
    while(!eof())
    {
        auto curly_close = consume_of_type(Token::CurlyClose);
        if(curly_close)
            return statement;

        auto label = parse_label();

        std::shared_ptr<Statement> node;
        {
            auto current_block_statement = m_current_block;
            node = parse_statement();
            m_current_block = current_block_statement;
        }

        if(!node)
            break;
        if(node->is_error())
            break;
        if(node->has_effect())
        {
            ASTNode::ErrorList errors;
            std::visit([&](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr(std::is_same_v<T, NoCaseLabel>)
                {
                    if(value == NoCaseLabel::Default)
                        statement->add_default_labeled_node(node);
                    else
                        statement->add_node(node);
                    return;
                }
                if constexpr(std::is_same_v<T, std::shared_ptr<CaseLabel>>)
                {
                    if(value->is_error())
                    {
                        errors = value->errors();
                        return;
                    }
                    statement->add_case_labeled_node(value, node);
                    return;
                }
            }, label);
            if(!errors.empty())
                return std::make_shared<BlockStatement>(errors);
        }

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

std::shared_ptr<IfStatement> EVOParser::parse_if_statement()
{
    auto if_keyword = consume_of_type(Token::Name);
    if(!if_keyword || if_keyword->value() != "if")
        return {};

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected '(' after 'if'"));

    auto condition = parse_expression();
    if(condition->is_error())
        return std::make_shared<IfStatement>(condition->errors());

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected ')' after 'if' condition"));

    auto true_statement = parse_statement();
    if(!true_statement || true_statement->is_error())
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected statement"));

    if(true_statement->requires_semicolon())
    {
        auto semicolon = consume_of_type(Token::Semicolon);
        if(!semicolon)
            return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected ';' after 'if' statement"));
    }

    size_t off = offset();
    auto else_keyword = consume_of_type(Token::Name);
    if(!else_keyword || else_keyword->value() != "else")
    {
        set_offset(off);
        return std::make_shared<IfStatement>(condition, true_statement, nullptr);
    }

    auto false_statement = parse_statement();
    if(!false_statement || false_statement->is_error())
        return std::make_shared<IfStatement>(ASTNode::Error(location(), "Expected statement"));

    return std::make_shared<IfStatement>(condition, true_statement, false_statement);
}

std::shared_ptr<WhileStatement> EVOParser::parse_while_statement()
{
    auto if_keyword = consume_of_type(Token::Name);
    if(!if_keyword || if_keyword->value() != "while")
        return {};

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<WhileStatement>(ASTNode::Error(location(), "Expected '(' after 'while'"));

    auto condition = parse_expression();
    if(condition->is_error())
        return std::make_shared<WhileStatement>(condition->errors());

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<WhileStatement>(ASTNode::Error(location(), "Expected ')' after 'while' condition"));

    auto statement = parse_statement();
    if(!statement)
        return std::make_shared<WhileStatement>(ASTNode::Error(location(), "Expected statement"));
    if(statement->is_error())
        return std::make_shared<WhileStatement>(statement->errors());

    return std::make_shared<WhileStatement>(condition, statement);
}

std::shared_ptr<ForStatement> EVOParser::parse_for_statement()
{
    auto if_keyword = consume_of_type(Token::Name);
    if(!if_keyword || if_keyword->value() != "for")
        return {};

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<ForStatement>(ASTNode::Error(location(), "Expected '(' after 'for'"));

    std::shared_ptr<Statement> initialization, condition, incrementation;
    do
    {
        {
            size_t off = offset();
            // NOTE: for(;;) case, also very hacky
            // until we have real empty statements
            auto token = consume_of_type(Token::Semicolon);
            if(token)
            {
                token = consume_of_type(Token::Semicolon);
                if(token)
                {
                    token = peek();
                    if(token && token->type() == Token::ParenClose)
                        break;
                }
            }
            set_offset(off);
        }

        initialization = parse_statement();
        if(initialization->is_error())
            return std::make_shared<ForStatement>(initialization->errors());
        if(initialization->requires_semicolon())
        {
            auto semicolon = consume_of_type(Token::Semicolon);
            if(!semicolon && !eof())
                return std::make_shared<ForStatement>(ASTNode::Error(location(), "Expected ';' after initialization statement"));
        }

        condition = parse_statement();
        if(condition->is_error())
            return std::make_shared<ForStatement>(condition->errors());
        if(condition->requires_semicolon())
        {
            auto semicolon = consume_of_type(Token::Semicolon);
            if(!semicolon && !eof())
                return std::make_shared<ForStatement>(ASTNode::Error(location(), "Expected ';' after condition statement"));
        }

        // FIXME: This is very hacky.
        incrementation = parse_statement();
        if(incrementation->is_error())
            incrementation = {};
    } while(false);

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<ForStatement>(ASTNode::Error(location(), "Expected ')'"));

    auto statement = parse_statement();
    if(!statement || statement->is_error())
        return std::make_shared<ForStatement>(ASTNode::Error(location(), "Expected statement"));

    return std::make_shared<ForStatement>(initialization, condition, incrementation, statement);
}

std::shared_ptr<SwitchStatement> EVOParser::parse_switch_statement()
{
    auto switch_keyword = consume_of_type(Token::Name);
    if(!switch_keyword || switch_keyword->value() != "switch")
        return {};

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<SwitchStatement>(ASTNode::Error(location(), "Expected '(' after 'switch'"));

    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<SwitchStatement>(expression->errors());

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<SwitchStatement>(ASTNode::Error(location(), "Expected ')' after 'switch' condition"));

    auto statement = parse_block_statement();
    if(!statement || statement->ASTGroupNode::is_error())
        return std::make_shared<SwitchStatement>(ASTNode::Error(location(), "Expected block statement"));

    return std::make_shared<SwitchStatement>(expression, statement);
}

std::shared_ptr<ReturnStatement> EVOParser::parse_return_statement()
{
    auto return_keyword = consume_of_type(Token::Name);
    if(!return_keyword || return_keyword->value() != "return")
        return {};

    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<ReturnStatement>(nullptr);
    
    return std::make_shared<ReturnStatement>(expression);
}

std::shared_ptr<ThrowStatement> EVOParser::parse_throw_statement()
{
    auto throw_keyword = consume_of_type(Token::Name);
    if(!throw_keyword || throw_keyword->value() != "throw")
        return {};

    auto expression = parse_expression();
    if(expression->is_error())
        return std::make_shared<ThrowStatement>(ASTNode::Error(location(), "Expected expression"));
    
    return std::make_shared<ThrowStatement>(expression);
}

std::shared_ptr<TryCatchStatement> EVOParser::parse_try_catch_statement()
{
    auto try_keyword = consume_of_type(Token::Name);
    if(!try_keyword || try_keyword->value() != "try")
        return {};

    // Unlike other languages, we allow all kinds of statements (not only block statements)
    auto try_statement = parse_statement();
    if(!try_statement)
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected statement"));
    if(try_statement->is_error())
        return std::make_shared<TryCatchStatement>(try_statement->errors());

    // Allow semicolon after non-block statements
    consume_of_type(Token::Semicolon);

    auto catch_keyword = consume_of_type(Token::Name);
    if(!catch_keyword || catch_keyword->value() != "catch")
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected 'catch'"));

    auto paren_open = consume_of_type(Token::ParenOpen);
    if(!paren_open)
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected '(' after 'catch'"));

    auto variable_name = consume_of_type(Token::Name);
    if(!variable_name)
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected variable name in 'catch'"));

    auto paren_close = consume_of_type(Token::ParenClose);
    if(!paren_close)
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected ')' after 'catch' variable name"));

    // Unlike other languages, we allow all kinds of statements (not only block statements)
    auto catch_statement = parse_statement();
    if(!catch_statement)
        return std::make_shared<TryCatchStatement>(ASTNode::Error(location(), "Expected statement"));
    if(catch_statement->is_error())
        return std::make_shared<TryCatchStatement>(catch_statement->errors());

    return std::make_shared<TryCatchStatement>(try_statement, catch_statement, variable_name->value());
}

std::shared_ptr<SimpleControlStatement> EVOParser::parse_simple_control_statement()
{
    auto keyword = consume_of_type(Token::Name);
    if(!keyword)
        return {};

    if(keyword->value() == "break")
        return std::make_shared<SimpleControlStatement>(SimpleControlStatement::Break);
    if(keyword->value() == "continue")
        return std::make_shared<SimpleControlStatement>(SimpleControlStatement::Continue);
    return {};
}

std::shared_ptr<Statement> EVOParser::parse_statement()
{
    // Empty statement
    auto semicolon = consume_of_type(Token::Semicolon);
    if(semicolon)
        return std::make_shared<BlockStatement>();

    size_t off = offset();
    std::shared_ptr<Statement> statement = parse_block_statement();
    if(!statement)
    {
        set_offset(off);
        statement = parse_simple_control_statement();
        if(!statement)
        {
            set_offset(off);
            statement = parse_return_statement();
            if(!statement)
            {
                set_offset(off);
                statement = parse_throw_statement();
                if(!statement)
                {
                    set_offset(off);
                    statement = parse_if_statement();
                    if(!statement)
                    {
                        set_offset(off);
                        statement = parse_while_statement();
                        if(!statement)
                        {
                            set_offset(off);
                            statement = parse_for_statement();
                            if(!statement)
                            {
                                set_offset(off);
                                statement = parse_try_catch_statement();
                                if(!statement)
                                {
                                    set_offset(off);
                                    statement = parse_switch_statement();
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
                            }
                        }
                    }
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
                return std::make_shared<Program>(ASTNode::Error(location(), "Expected ';' after statement"));
        }
    }
    return program;
}

}
