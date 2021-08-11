#include "libevoscript/ExecutionContext.h"
#include <libevoscript/AST.h>

#include <cassert>
#include <memory>

namespace evo::script
{

Value IntegerLiteral::evaluate(Runtime&) const
{
    return Value::new_int(m_value);
}

Value Identifier::evaluate(Runtime& rt) const
{
    auto object = rt.current_execution_context().local_scope_object()->value().to_object(rt);
    if(rt.has_exception())
        return {}; // Local scope is not an object (unlikely to happen)

    auto memory_object = object->get(m_name).to_reference(rt);
    if(rt.has_exception())
        return {}; // MemoryObject is not a reference

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

    ScopedExecutionContext context(rt, callable.container());
    if(rt.has_exception())
        return {}; // 'this' is not an object

    return callable.call(rt);
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

    reference->value().assign(rhs);
    return Value::new_reference(reference);
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
    }
    return val;
}

std::ostream& operator<<(std::ostream& stream, ASTNode& node)
{
    return stream << node.to_string();
}

}
