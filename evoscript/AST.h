#pragma once

#include <compare>
#include <evoscript/EvalResult.h>
#include <evoscript/SourceLocation.h>
#include <evoscript/StringId.h>
#include <evoscript/Value.h>

#include <memory>
#include <vector>
#include <sstream>

namespace evo::script
{

class ASTNode
{
public:
    using ErrorMessage = std::string;

    struct Error
    {
        SourceSpan location;
        std::string message;

        Error() = default;
        Error(SourceSpan const& _location, std::string const& message_)
        : location(std::move(_location)), message(std::move(message_)) {}
    };

    class ErrorList
    {
    public:
        using Type = std::vector<Error>;

        ErrorList() = default;

        ErrorList(Error const& error)
        : m_errors({error}) {}

        ErrorList(std::vector<Error> const& errors)
        : m_errors(errors) {}

        Type::const_iterator begin() const { return m_errors.begin(); }
        Type::const_iterator end() const { return m_errors.end(); }
        bool empty() const { return m_errors.empty(); }
        void add(Error const& error) { m_errors.push_back(error); }

        void print(std::ostream& out, std::istream& in) const;

    private:
        Type m_errors;
    };

    ASTNode(ErrorList const& error)
    : m_errors(error) {}

    ASTNode() = default;
    ASTNode(ASTNode const&) = delete;

    virtual EvalResult evaluate(Runtime&) const = 0;

    bool is_error() const { return !m_errors.empty(); }
    ErrorList errors() const { return m_errors; }

    virtual std::string to_string() const { return "??"; }

protected:
    ErrorList m_errors;
};

template<class T>
class ASTGroupNode : public ASTNode
{
public:
    ASTGroupNode(ErrorList const& error)
    : ASTNode(error) {}

    ASTGroupNode() = default;

    void add_node(std::shared_ptr<T> node)
    {
        assert(node);
        m_nodes.push_back(node);
        if(node->is_error())
        {
            for(auto& error: node->errors())
                m_errors.add(error);
        }
    }

    virtual std::string to_string() const override
    {
        std::string out = "ASTGroupNode(";
        for(auto& it: m_nodes)
        {
            out += it->to_string() + "; ";
        }
        return out + ")";
    }

protected:
    std::vector<std::shared_ptr<T>> m_nodes;
};

class Expression : public ASTNode
{
public:
    Expression(ErrorList const& error)
    : ASTNode(error) {}

    Expression() = default;

    virtual EvalResult evaluate(Runtime&) const override { return {}; }

    // has effect = modifies environment (local/global/this object)
    virtual bool has_effect() const { return true; }
};

class IntegerLiteral : public Expression
{
public:
    IntegerLiteral(ErrorList const& error)
    : Expression(error) {}

    IntegerLiteral(Value::IntType value)
    : m_value(value) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool has_effect() const override { return false; }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "IntegerLiteral(" + std::to_string(m_value) + ")";
    }

private:
    Value::IntType m_value;
};

class StringLiteral : public Expression
{
public:
    StringLiteral(ErrorList const& error)
    : Expression(error) {}

    StringLiteral(std::string const& value)
    : m_value(value) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool has_effect() const override { return false; }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "StringLiteral(\"" + m_value + "\")";
    }

private:
    std::string m_value;
};

class Identifier : public Expression
{
public:
    Identifier(ErrorList const& error)
    : Expression(error) {}

    Identifier(StringId name)
    : m_name(name) {}

    virtual EvalResult evaluate(Runtime&) const override;
    // TODO: has_effect = false

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "Identifier(" + m_name.string() + ")";
    }

private:
    StringId m_name;
};

class SpecialValue : public Expression
{
public:
    enum Type
    {
        This,
        Global,
        Null,
        True,
        False,
        Undefined,
        Local
    };

    SpecialValue(ErrorList const& error)
    : Expression(error) {}

    SpecialValue(Type type)
    : m_type(type) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool has_effect() const override { return false; }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "SpecialValue(" + std::to_string(int(m_type)) + ")";
    }

private:
    Type m_type;
};

class MemberExpression : public Expression
{
public:
    MemberExpression(ErrorList const& error)
    : Expression(error) {}

    MemberExpression(std::shared_ptr<Expression> expression, std::string const& name)
    : m_expression(expression), m_name(name) {}

    virtual EvalResult evaluate(Runtime&) const override;
    // TODO: has_effect = false

    std::shared_ptr<Expression> this_expression() const { return m_expression; }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "MemberExpression(" + m_expression->to_string() + "." + m_name.string() + ")";
    }

private:
    std::shared_ptr<Expression> m_expression;
    StringId m_name;
};

class FunctionCall : public Expression
{
public:
    FunctionCall(ErrorList const& error)
    : Expression(error) {}

    FunctionCall(std::shared_ptr<Expression> callable, std::vector<std::shared_ptr<Expression>> const& arguments)
    : m_callable(callable), m_arguments(std::move(arguments)) {}

    virtual EvalResult evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        std::string output = "FunctionCall(" + m_callable->to_string() + "(";
        for(auto& it: m_arguments)
            output += it->to_string() + ", ";
        return output + "))";
    }

private:
    std::shared_ptr<Expression> m_callable;
    std::vector<std::shared_ptr<Expression>> m_arguments;
};

class Subscript : public Expression
{
public:
    Subscript(ErrorList const& error)
    : Expression(error) {}

    Subscript(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> subscript)
    : m_expression(expression), m_subscript(subscript) {}

    virtual EvalResult evaluate(Runtime&) const override;

private:
    std::shared_ptr<Expression> m_expression;
    std::shared_ptr<Expression> m_subscript;
};

class NewExpression : public Expression
{
public:
    NewExpression(ErrorList const& error)
    : Expression(error) {}

    NewExpression(std::shared_ptr<Expression> const& name, std::vector<std::shared_ptr<Expression>> const& arguments)
    : m_name(name), m_arguments(std::move(arguments)) {}

    virtual EvalResult evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        std::string output = "NewExpression(new " + m_name->to_string() + "(";
        for(auto& it: m_arguments)
            output += it->to_string() + ", ";
        return output + "))";
    }

private:
    std::shared_ptr<Expression> m_name;
    std::vector<std::shared_ptr<Expression>> m_arguments;
};

class UnaryExpression : public Expression
{
public:
    enum Operation
    {
        Not,        // !
        BitwiseNot, // ~
        Minus,      // -
        Plus,       // +
        Increment,  // ++
        Decrement,  // --
        PostfixIncrement,   // ++
        PostfixDecrement,   // --
    };

    UnaryExpression(ErrorList const& error)
    : Expression(error) {}

    UnaryExpression(std::shared_ptr<Expression> expression, Operation operation)
    : m_expression(expression), m_operation(operation) {}

    virtual EvalResult evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "UnaryExpression(" + m_expression->to_string() + ")";
    }

private:
    std::shared_ptr<Expression> m_expression;
    Operation m_operation;
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(ErrorList const& error)
    : Expression(error) {}

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "BinaryExpression(" + m_lhs->to_string() + " @ " + m_rhs->to_string() + ")";
    }

protected:
    BinaryExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
    : m_lhs(lhs), m_rhs(rhs) {}

    std::shared_ptr<Expression> m_lhs;
    std::shared_ptr<Expression> m_rhs;
};

// lhs must be a reference, returns modified lhs (equivalent of {lhs = op(lhs, rhs); return lhs})
class AssignmentExpression : public BinaryExpression
{
public:
    enum Operation
    {
        Assign,     // =
        Add,        // +=
        Subtract,   // -=
        Multiply,   // *=
        Divide,     // /=
        Modulo,     // %=
    };

    AssignmentExpression(ErrorList const& error)
    : BinaryExpression(error) {}

    AssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operation operation)
    : BinaryExpression(lhs, rhs), m_operation(operation) {}

    virtual EvalResult evaluate(Runtime&) const override;

    static std::string operation_string(Operation op)
    {
        switch(op)
        {
            case Assign:    return "=";
            case Add:       return "+=";
            case Subtract:  return "-=";
            case Multiply:  return "*=";
            case Divide:    return "/=";
            case Modulo:    return "%=";
        }
        return "?=";
    }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "AssignmentExpression(" + m_lhs->to_string() + " " + operation_string(m_operation) + " " + m_rhs->to_string() + ")";
    }

private:
    Operation m_operation;
};

// lhs + rhs can be values, return result of op(lhs, rhs)
class NormalBinaryExpression : public BinaryExpression
{
public:
    enum Operation
    {
        Add,            // +
        Subtract,       // -
        Multiply,       // *
        Divide,         // /
        Modulo,         // %
        Equal,          // ==
        NonEqual,       // !=
        Greater,        // >
        GreaterEqual,   // >=
        Less,           // <
        LessEqual,      // <=
        And,            // &&
        Or,             // ||

    };

    NormalBinaryExpression(ErrorList const& error)
    : BinaryExpression(error) {}

    NormalBinaryExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operation operation)
    : BinaryExpression(lhs, rhs), m_operation(operation) {}

    virtual EvalResult evaluate(Runtime&) const override;

    static std::string operation_string(Operation op)
    {
        switch(op)
        {
            case Add:           return "+";
            case Subtract:      return "-";
            case Multiply:      return "*";
            case Divide:        return "/";
            case Modulo:        return "%";
            case Equal:         return "==";
            case NonEqual:      return "!=";
            case Greater:       return "?";
            case GreaterEqual:  return ">=";
            case Less:          return "<";
            case LessEqual:     return "<=";
            case And:           return "&&";
            case Or:            return "||";
        }
        return "?";
    }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "NormalBinaryExpression(" + m_lhs->to_string() + " " + operation_string(m_operation) + " " + m_rhs->to_string() + ")";
    }

private:
    Operation m_operation;
};

class BlockStatement;

class FunctionExpression : public Expression
{
public:
    FunctionExpression(ErrorList const& error)
    : Expression(error) {}

    FunctionExpression(StringId name, std::shared_ptr<BlockStatement> body, std::vector<StringId> const& arg_names)
    : m_name(name), m_body(body), m_arg_names(arg_names) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual std::string to_string() const override;

    std::string name() const { return m_name.string(); }

private:
    StringId m_name;
    std::shared_ptr<BlockStatement> m_body;
    std::vector<StringId> m_arg_names;
};

class Statement : public ASTNode
{
public:
    Statement(ErrorList const& error)
    : ASTNode(error) {}

    Statement() = default;

    virtual bool requires_semicolon() const { return true; }
    virtual bool has_effect() const { return true; }
};

class ExpressionStatement : public Statement
{
public:
    ExpressionStatement(ErrorList const& error)
    : Statement(error) {}

    ExpressionStatement(std::shared_ptr<Expression> expression)
    : m_expression(expression) { assert(expression); }

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool has_effect() const override { return m_expression->has_effect(); }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "ExpressionStatement(" + m_expression->to_string() + ")";
    }

private:
    std::shared_ptr<Expression> m_expression;
};

class BlockStatement : public ASTGroupNode<Statement>, public Statement
{
public:
    BlockStatement(ErrorList const& error)
    : Statement(error) {}

    BlockStatement() = default;

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override { return false; }

    virtual std::string to_string() const override
    {
        return "BlockStatement { TODO }";
    }
};

class IfStatement : public Statement
{
public:
    IfStatement(ErrorList const& error)
    : Statement(error) {}

    IfStatement(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> true_statement, std::shared_ptr<Statement> false_statement)
    : m_condition(condition), m_true_statement(true_statement), m_false_statement(false_statement) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override
    {
        if(m_false_statement)
            return m_false_statement->requires_semicolon();
        assert(m_true_statement);
        return m_true_statement->requires_semicolon();
    }

private:
    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Statement> m_true_statement;
    std::shared_ptr<Statement> m_false_statement;
};

class WhileStatement : public Statement
{
public:
    WhileStatement(ErrorList const& error)
    : Statement(error) {}

    WhileStatement(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> statement)
    : m_condition(condition), m_statement(statement) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override
    {
        assert(m_statement);
        return m_statement->requires_semicolon();
    }

private:
    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Statement> m_statement;
};

class ForStatement : public Statement
{
public:
    ForStatement(ErrorList const& error)
    : Statement(error) {}

    ForStatement(std::shared_ptr<Statement> initialization, std::shared_ptr<Statement> condition, std::shared_ptr<Statement> incrementation, std::shared_ptr<Statement> statement)
    : m_initialization(initialization), m_condition(condition), m_incrementation(incrementation), m_statement(statement) {}

    virtual EvalResult evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override
    {
        assert(m_statement);
        return m_statement->requires_semicolon();
    }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "ForStatement(for(" + (m_initialization ? m_initialization->to_string() : "null") + ";"
            + (m_condition ? m_condition->to_string() : "null") + ";"
            + (m_incrementation ? m_incrementation->to_string() : "null") + ") {" + m_statement->to_string() + "})";
    }

private:
    std::shared_ptr<Statement> m_initialization;
    std::shared_ptr<Statement> m_condition;
    std::shared_ptr<Statement> m_incrementation;
    std::shared_ptr<Statement> m_statement;
};

class ReturnStatement : public Statement
{
public:
    ReturnStatement(ErrorList const& error)
    : Statement(error) {}

    ReturnStatement(std::shared_ptr<Expression> expression)
    : m_expression(expression) {}

    virtual EvalResult evaluate(Runtime&) const override;

private:
    std::shared_ptr<Expression> m_expression;
};

class SimpleControlStatement : public Statement
{
public:
    enum Operation
    {
        Break,
        Continue
    };

    SimpleControlStatement(ErrorList const& error)
    : Statement(error) {}

    SimpleControlStatement(Operation op)
    : m_operation(op) {}

    virtual EvalResult evaluate(Runtime&) const override;

private:
    Operation m_operation;
};

class Declaration : public Statement
{
public:
    Declaration() = default;

    Declaration(ErrorList const& error)
    : Statement(error) {}
};

class VariableDeclaration : public Declaration
{
public:
    enum Type
    {
        Let,
        Const
    };

    VariableDeclaration(ErrorList const& error)
    : Declaration(error) {}

    VariableDeclaration(Type type, StringId name, std::shared_ptr<Expression> initializer)
    : m_type(type), m_name(name), m_initializer(initializer) {}

    virtual EvalResult evaluate(Runtime&) const override;

private:
    Type m_type { Let };
    StringId m_name;
    std::shared_ptr<Expression> m_initializer;
};

class FunctionDeclaration : public Declaration
{
public:
    FunctionDeclaration(ErrorList const& error)
    : Declaration(error) {}

    FunctionDeclaration(std::shared_ptr<FunctionExpression> expression)
    : m_expression(expression) {}

    virtual EvalResult evaluate(Runtime&) const override;

    virtual bool requires_semicolon() const override { return false; }

private:
    std::shared_ptr<FunctionExpression> m_expression;
};

class Program : public ASTGroupNode<Statement>
{
public:
    Program(ErrorList const& error)
    : ASTGroupNode<Statement>(error) {}

    Program() = default;

    virtual EvalResult evaluate(Runtime&) const override;
};

std::ostream& operator<<(std::ostream& stream, ASTNode& node);

}
