#pragma once

#include <evoscript/AST.h>
#include <evoscript/SourceLocation.h>

#include <cassert>
#include <ostream>
#include <type_traits>
#include <variant>
#include <vector>

namespace evo::script
{

class Token
{
public:
    enum Type
    {
        AssignmentOperator,     // (all assignment operators)
        Comma,                  // ,
        Dot,                    // .
        Name,                   // /([a-zA-Z_]?[a-zA-Z0-9_]*)/
        NormalOperator,         // (all operators other than assignment)
        Number,                 // /([0-9])/
        ParenOpen,              // (
        ParenClose,             // )
        Semicolon,              // ;
        String,                 // /["']([^"']*)["']/
        CurlyOpen,              // {
        CurlyClose,             // }
        BraceOpen,              // [
        BraceClose,             // ]
        Colon,                  // :
        Invalid                 // (others)
    };

    Token(Type type, std::string const& value, SourceLocation const& start = {}, SourceLocation const& end = {})
    : m_type(type), m_value(value), m_start(start), m_end(end) {}

    Type type() const { return m_type; }
    std::string value() const { return m_value; }
    SourceLocation start() const { return m_start; }
    SourceLocation end() const { return m_end; }

private:
    Type m_type;
    std::string m_value;
    SourceLocation m_start;
    SourceLocation m_end;
};

class Parser
{
public:
    Parser(std::vector<Token> const& tokens)
    : m_tokens(tokens) {}

    Token const* consume()
    {
        auto token = peek();
        if(!token)
            return nullptr;
        m_offset++;
        return token;
    }

    Token const* consume_of_type(Token::Type type)
    {
        auto token = peek();
        if(!token || token->type() != type)
            return nullptr;
        m_offset++;
        return token;
    }

    Token const* peek() const
    {
        return !eof() ? &m_tokens[m_offset] : nullptr;
    }

    bool eof() const
    {
        return m_offset >= m_tokens.size();
    }

    size_t offset() const { return m_offset; }
    void set_offset(size_t offset) { m_offset = offset; }

    SourceSpan location() const
    { 
        auto* token = peek();
        return token ? SourceSpan{token->start(), token->value().size()} : (
            m_tokens.empty() ? SourceSpan{} : SourceSpan{m_tokens.back().end(), 1}
        );
    }

private:
    std::vector<Token> m_tokens;
    size_t m_offset = 0;
};

class EVOParser : public Parser
{
public:
    std::shared_ptr<FunctionExpression> parse_function_expression();
    std::shared_ptr<IntegerLiteral> parse_integer_literal();
    std::shared_ptr<StringLiteral> parse_string_literal();
    std::shared_ptr<Expression> parse_identifier();
    std::shared_ptr<Expression> parse_special_value();
    std::shared_ptr<Expression> parse_primary_expression(); // ( expression ) | integer_literal | string_literal | identifier | special_value
    std::shared_ptr<Expression> parse_postfix_expression(); // .name | (arg1, arg2, ...) | expr++ | expr--
    std::shared_ptr<Expression> parse_new_expression();
    std::shared_ptr<Expression> parse_unary_expression();
    std::shared_ptr<Expression> parse_multiplicative_expression();
    std::shared_ptr<Expression> parse_additive_expression();
    std::shared_ptr<Expression> parse_comparison_expression();
    std::shared_ptr<Expression> parse_logical_and_expression();
    std::shared_ptr<Expression> parse_logical_or_expression();
    std::shared_ptr<Expression> parse_assignment_expression();
    std::shared_ptr<Expression> parse_expression();

    std::shared_ptr<Declaration>         parse_declaration();
    std::shared_ptr<VariableDeclaration> parse_variable_declaration();
    std::shared_ptr<FunctionDeclaration> parse_function_declaration();

    std::shared_ptr<Statement>              parse_statement();
    std::shared_ptr<Statement>              parse_expression_statement();

    enum class NoCaseLabel { Default, None };
    using Label = std::variant<std::shared_ptr<CaseLabel>, NoCaseLabel>;
    Label                                   parse_label();
    std::shared_ptr<BlockStatement>         parse_block_statement();
    std::shared_ptr<IfStatement>            parse_if_statement();
    std::shared_ptr<WhileStatement>         parse_while_statement();
    std::shared_ptr<ForStatement>           parse_for_statement();
    std::shared_ptr<SwitchStatement>        parse_switch_statement();
    std::shared_ptr<ReturnStatement>        parse_return_statement();
    std::shared_ptr<SimpleControlStatement> parse_simple_control_statement();
    std::shared_ptr<Program>                parse_program();

    // Postfix helpers
    std::shared_ptr<Expression> parse_member_name(std::shared_ptr<Expression> lhs); // .name
    std::shared_ptr<Expression> parse_argument_list(std::shared_ptr<Expression> lhs); // (arg1, arg2, ...)
    std::shared_ptr<Expression> parse_subscript_value(std::shared_ptr<Expression> lhs); // [value]
    std::shared_ptr<Expression> parse_postfix_operator(std::shared_ptr<Expression> lhs); // ++ | --

    BlockStatement* m_current_block { nullptr };
};

}
