#pragma once

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
    : m_error_message(message) {}

    ASTNode() = default;

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
class ASTGroupNode : public ASTNode
{
public:
    ASTGroupNode(_ErrorTag tag, ErrorMessage message)
    : ASTNode(tag, message) {}

    ASTGroupNode() = default;

    void add_node(std::shared_ptr<T> node)
    {
        m_nodes.push_back(node);
    }

    virtual bool is_error() const override
    {
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
    Expression(_ErrorTag tag, ErrorMessage message)
    : ASTNode(tag, message) {}

    Expression() = default;

    virtual Value evaluate(Runtime&) const override { return {}; }
};

class IntegerLiteral : public Expression
{
public:
    IntegerLiteral(_ErrorTag tag, ErrorMessage message)
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

class Identifier : public Expression
{
public:
    Identifier(_ErrorTag tag, ErrorMessage message)
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
        Undefined
    };

    SpecialValue(_ErrorTag tag, ErrorMessage message)
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
    MemberExpression(_ErrorTag tag, ErrorMessage message)
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
    FunctionCall(_ErrorTag tag, ErrorMessage message)
    : Expression(tag, message) {}

    FunctionCall(std::shared_ptr<Expression> callable)
    : m_callable(callable) {}

    virtual Value evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "FunctionCall(" + m_callable->to_string() + "())";
    }

private:
    std::shared_ptr<Expression> m_callable;
};

class AssignmentExpression : public Expression
{
public:
    AssignmentExpression(_ErrorTag tag, ErrorMessage message)
    : Expression(tag, message) {}

    AssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
    : m_lhs(lhs), m_rhs(rhs) {}

    virtual Value evaluate(Runtime&) const override;

    virtual std::string to_string() const override
    {
        if(is_error())
            return ASTNode::to_string();

        return "AssignmentExpression(" + m_lhs->to_string() + " = " + m_rhs->to_string() + ")";
    }

private:
    std::shared_ptr<Expression> m_lhs;
    std::shared_ptr<Expression> m_rhs;
};

class Statement : public ASTNode
{
public:
    Statement(_ErrorTag tag, ErrorMessage message)
    : ASTNode(tag, message) {}

    Statement() = default;
};

class ExpressionStatement : public Statement
{
public:
    ExpressionStatement(_ErrorTag tag, ErrorMessage message)
    : Statement(tag, message) {}

    ExpressionStatement(std::shared_ptr<Expression> expression)
    : m_expression(expression) {}

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

class Program : public ASTGroupNode<Statement>
{
public:
    Program(_ErrorTag tag, ErrorMessage message)
    : ASTGroupNode<Statement>(tag, message) {}

    Program() = default;

    virtual Value evaluate(Runtime&) const override;
};

std::ostream& operator<<(std::ostream& stream, ASTNode& node);

}
