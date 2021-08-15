#include <libevoscript/AST.h>

#include <libevoscript/AbstractOperations.h>
#include <libevoscript/ExecutionContext.h>

#include <cassert>
#include <iostream>
#include <memory>

namespace evo::script
{

Value IntegerLiteral::evaluate(Runtime&) const
{
    return Value::new_int(m_value);
}

Value StringLiteral::evaluate(Runtime&) const
{
    return Value::new_string(m_value);
}

Value Identifier::evaluate(Runtime& rt) const
{
    auto object = rt.current_execution_context().local_scope_object()->value().to_object(rt);
    if(rt.has_exception())
        return {}; // Local scope is not an object (unlikely to happen)

    auto memory_object = object->get(m_name).to_reference(rt);
    if(rt.has_exception())
        return {}; // MemoryObject is not a reference

    if(memory_object->value().type() == Value::Type::Undefined)
    {
        // Not in local scope; try searching global scope
        // TODO: This means that not existing "local" variables will
        // be created on global object!
        auto global_object = rt.global_object()->value().to_object(rt);
        if(rt.has_exception())
            return {}; // Global scope is not an object (unlikely to happen)

        auto value = global_object->get(m_name);
        if(!value.is_reference()) 
        {
            return value;
        }
        
        memory_object = value.to_reference(rt);
        assert(!rt.has_exception());
    }

    return Value::new_reference(memory_object);
}

Value SpecialValue::evaluate(Runtime& rt) const
{
    switch(m_type)
    {
    case This:
        return Value::new_reference(MemoryValue::create_existing_object(rt.current_execution_context().this_object()));
    case Null:
        return Value::null();
    case Undefined:
        return Value::undefined();
    default:
        assert(false);
    }
}

Value MemberExpression::evaluate(Runtime& rt) const
{
    auto value = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating LHS

    auto object = value.to_object(rt);
    if(rt.has_exception())
        return {}; // LHS of member expression is not an object

    auto member = object->get(m_name);
    if(rt.has_exception())
        return {}; // get() failed (e.g nonexisting objects are not tolerated)

    member.set_container(object);
    return member;
}

Value FunctionCall::evaluate(Runtime& rt) const
{
    auto callable = m_callable->evaluate(rt);
    if(rt.has_exception())
        return {}; // failed to evaluate callable
    
    std::vector<Value> arguments;
    for(auto& expr: m_arguments)
    {
        auto value = expr->evaluate(rt);
        if(rt.has_exception())
            return {}; // failed to evaluate argument;

        arguments.push_back(value.dereferenced());
    }

    ScopedExecutionContext context(rt, callable.container());
    if(rt.has_exception())
        return {}; // 'this' is not an object

    return callable.call(rt, arguments);
}

Value UnaryExpression::evaluate(Runtime& rt) const
{
    auto value = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating expression

    Value result;
    switch(m_operation)
    {
        case Not:
            result = abstract::not_(rt, value);
            break;
        case BitwiseNot:
            result = abstract::bitwise_not(rt, value);
            break;
        case Minus:
            result = abstract::minus(rt, value);
            break;
        case Plus:
            result = abstract::plus(rt, value);
            break;
        case Increment:
            result = abstract::prefix_increment(rt, value);
            break;
        case Decrement:
            result = abstract::prefix_decrement(rt, value);
            break;
        default:
            assert(false);
    }

    return result;
}

Value AssignmentExpression::evaluate(Runtime& rt) const
{
    auto lhs = m_lhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating LHS
    
    auto rhs = m_rhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating RHS

    auto reference = lhs.to_reference(rt);
    if(rt.has_exception())
        return {}; // LHS is not a reference

    auto& lhs_value = reference->value();

    Value result;
    switch(m_operation)
    {
        case Assign:
            result = rhs;
            break;
        case Add:
            result = abstract::add(rt, lhs_value, rhs);
            break;
        case Subtract:
            result = abstract::subtract(rt, lhs_value, rhs);
            break;
        case Multiply:
            result = abstract::multiply(rt, lhs_value, rhs);
            break;
        case Divide:
            result = abstract::divide(rt, lhs_value, rhs);
            break;
        case Modulo:
            result = abstract::modulo(rt, lhs_value, rhs);
            break;
        default:
            assert(false);
    }

    if(rt.has_exception())
        return {};

    lhs_value.assign(result);
    return Value::new_reference(reference);
}

Value NormalBinaryExpression::evaluate(Runtime& rt) const
{
    auto lhs = m_lhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating LHS
    
    auto rhs = m_rhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating RHS

    Value result;
    switch(m_operation)
    {
        case Add:
            result = abstract::add(rt, lhs, rhs);
            break;
        case Subtract:
            result = abstract::subtract(rt, lhs, rhs);
            break;
        case Multiply:
            result = abstract::multiply(rt, lhs, rhs);
            break;
        case Divide:
            result = abstract::divide(rt, lhs, rhs);
            break;
        case Modulo:
            result = abstract::modulo(rt, lhs, rhs);
            break;
        default:
            assert(false);
    }

    return result;
}

Value ExpressionStatement::evaluate(Runtime& rt) const
{
    return m_expression->evaluate(rt);
}

Value Program::evaluate(Runtime& rt) const
{
    Value val;
    for(auto& it: m_nodes)
    {
        val = it->evaluate(rt);
        if(rt.has_exception())
            return {};
    }
    return val;
}

std::ostream& operator<<(std::ostream& stream, ASTNode& node)
{
    return stream << node.to_string();
}

}
