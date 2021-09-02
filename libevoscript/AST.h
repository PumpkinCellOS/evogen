#pragma once

#include <compare>
#include <libevoscript/Value.h>
#include <libevoscript/Runtime.h>

#include <memory>
#include <vector>

namespace evo::script
{

class ASTNode
{
public:
    using ErrorMessage = std::string;

    struct _ErrorTag {}; static constexpr _ErrorTag Error {};

    ASTNode(_ErrorTag tag, ErrorMessage const& message)
    : m_error_message(message)
    {

    }

    ASTNode() = default;
    ASTNode(ASTNode const&) = delete;

    virtual Value evaluate(Runtime&) const = 0;

    virtual bool is_error() const { return !m_error_message.empty(); }
    virtual std::string error_message() const { return is_error() ? m_error_message + "\n" : ""; }

    virtual std::string to_string() const
    {
        if(is_error())
            return "SyntaxError(" + error_message() + ")";
        return "";
    }

private:
    std::string m_error_message;
};

template<class T>
class ASTGroupNode : virtual public ASTNode
{
public:
    ASTGroupNode(_ErrorTag tag, ErrorMessage const& message)
    : ASTNode(tag, message) {}

    ASTGroupNode() = default;

    void add_node(std::shared_ptr<T> node)
    {
        assert(node);
        m_nodes.push_back(node);
    }

    virtual bool is_error() const override
    {
        if(ASTNode::is_error())
            return true;
        for(auto& it: m_nodes)
        {
            if(it->is_error())
                return true;
        }
        return false;
    }
    
    virtual std::string error_message() const override
    {
        // FIXME: This is bad
        std::string messages = ASTNode::error_message();

        for(auto& it: m_nodes)
        {
            if(it->is_error())
                messages += it->error_message() + "\n";
        }
        return messages;
    }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

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
    Expression(_ErrorTag tag, ErrorMessage const& message)
    : ASTNode(tag, message) {}

    Expression() = default;

    virtual Value evaluate(Runtime&) const override { return {}; }
};

class IntegerLiteral : public Expression
{
public:
    IntegerLiteral(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    IntegerLiteral(int value)
    : m_value(value) {}

    virtual Value evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "IntegerLiteral(" + std::to_string(m_value) + ")";
    }

private:
    int m_value;
};

class StringLiteral : public Expression
{
public:
    StringLiteral(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    StringLiteral(std::string const& value)
    : m_value(value) {}

    virtual Value evaluate(Runtime&) const override;

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
    Identifier(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    Identifier(std::string const& name)
    : m_name(name) {}

    virtual Value evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "Identifier(" + m_name + ")";
    }

private:
    std::string m_name;
};

class SpecialValue : public Expression
{
public:
    enum Type
    {
        This,
        Null,
        True,
        False,
        Undefined
    };

    SpecialValue(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    SpecialValue(Type type)
    : m_type(type) {}

    virtual Value evaluate(Runtime&) const override;

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
    MemberExpression(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    MemberExpression(std::shared_ptr<Expression> expression, std::string const& name)
    : m_expression(expression), m_name(name) {}

    virtual Value evaluate(Runtime&) const override;

    std::shared_ptr<Expression> this_expression() const { return m_expression; }

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "MemberExpression(" + m_expression->to_string() + "." + m_name + ")";
    }

private:
    std::shared_ptr<Expression> m_expression;
    std::string m_name;
};

class FunctionCall : public Expression
{
public:
    FunctionCall(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    FunctionCall(std::shared_ptr<Expression> callable, std::vector<std::shared_ptr<Expression>> const& arguments)
    : m_callable(callable), m_arguments(std::move(arguments)) {}

    virtual Value evaluate(Runtime&) const override;

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

    UnaryExpression(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

    UnaryExpression(std::shared_ptr<Expression> expression, Operation operation)
    : m_expression(expression), m_operation(operation) {}

    virtual Value evaluate(Runtime&) const override;

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
    BinaryExpression(_ErrorTag tag, ErrorMessage const& message)
    : Expression(tag, message) {}

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

    AssignmentExpression(_ErrorTag tag, ErrorMessage const& message)
    : BinaryExpression(tag, message) {}

    AssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operation operation)
    : BinaryExpression(lhs, rhs), m_operation(operation) {}

    virtual Value evaluate(Runtime&) const override;

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

    NormalBinaryExpression(_ErrorTag tag, ErrorMessage const& message)
    : BinaryExpression(tag, message) {}

    NormalBinaryExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operation operation)
    : BinaryExpression(lhs, rhs), m_operation(operation) {}

    virtual Value evaluate(Runtime&) const override;

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

class Statement : public ASTNode
{
public:
    Statement(_ErrorTag tag, ErrorMessage const& message)
    : ASTNode(tag, message) {}

    Statement() = default;

    virtual bool requires_semicolon() const { return true; }
};

class ExpressionStatement : public Statement
{
public:
    ExpressionStatement(_ErrorTag tag, ErrorMessage const& message)
    : Statement(tag, message)
    {

    }

    ExpressionStatement(std::shared_ptr<Expression> expression)
    : m_expression(expression) { assert(expression); }

    virtual Value evaluate(Runtime&) const override;

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
    BlockStatement(_ErrorTag tag, ErrorMessage const& message)
    : ASTGroupNode<Statement>(tag, message) {}

    BlockStatement() = default;

    virtual Value evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override { return false; }
};

class IfStatement : public Statement
{
public:
    IfStatement(_ErrorTag tag, ErrorMessage const& message)
    : Statement(tag, message) {}

    // TODO: else
    IfStatement(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> true_statement)
    : m_condition(condition), m_true_statement(true_statement) {}

    virtual Value evaluate(Runtime&) const override;
    virtual bool requires_semicolon() const override { return false; }

private:
    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Statement> m_true_statement;
};

class Program : public ASTGroupNode<Statement>
{
public:
    Program(_ErrorTag tag, ErrorMessage const& message)
    : ASTGroupNode<Statement>(tag, message) {}

    Program() = default;

    virtual Value evaluate(Runtime&) const override;
};

std::ostream& operator<<(std::ostream& stream, ASTNode& node);

}
