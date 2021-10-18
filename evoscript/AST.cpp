#include "evoscript/EvalResult.h"
#include <evoscript/AST.h>

#include <evoscript/AbstractOperations.h>
#include <evoscript/ExecutionContext.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/objects/ASTFunction.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/StringObject.h>

#include <cassert>
#include <iostream>
#include <memory>

namespace evo::script
{

void ASTNode::ErrorList::print(std::ostream& output, std::istream& input) const
{
    for(auto& it: *this)
    {
        output << "\e[1m" << it.location.start << "\e[m: " << it.message << std::endl;
        display_source_range(output, input, it.location);
    }
}

EvalResult IntegerLiteral::evaluate(Runtime&) const
{
    return Value::new_int(m_value);
}

EvalResult StringLiteral::evaluate(Runtime&) const
{
    return new_object_value<StringObject>(m_value);
}

EvalResult Identifier::evaluate(Runtime& rt) const
{
    auto [container, reference] = rt.resolve_identifier(m_name);
    if(!reference)
    {
        rt.throw_exception("Non-existing reference to " + m_name.string());
        return {};
    }
    
    auto new_value = Value::new_reference(reference);
    new_value.set_container(container);
    return new_value;
}

EvalResult SpecialValue::evaluate(Runtime& rt) const
{
    switch(m_type)
    {
    case This:
        return Value::new_reference(MemoryValue::create_existing_object(rt.current_execution_context().this_object()));
    case Global:
        return Value::new_reference(MemoryValue::create_existing_object(rt.global_object()));
    case Null:
        return Value::null();
    case True:
        return Value::new_bool(true);
    case False:
        return Value::new_bool(false);
    case Undefined:
        return Value::undefined();
    default:
        assert(false);
        return {};
    }
}

EvalResult MemberExpression::evaluate(Runtime& rt) const
{
    Value value = m_expression->evaluate(rt);
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

EvalResult FunctionCall::evaluate(Runtime& rt) const
{
    Value callable = m_callable->evaluate(rt);
    if(rt.has_exception())
        return {}; // failed to evaluate callable
    
    std::vector<Value> arguments;
    for(auto& expr: m_arguments)
    {
        Value value = expr->evaluate(rt);
        if(rt.has_exception())
            return {}; // failed to evaluate argument

        arguments.push_back(value.dereferenced());
    }

    return callable.call(rt, arguments);
}

EvalResult Subscript::evaluate(Runtime& rt) const
{
    Value value = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {};
    Value subscript_value = m_subscript->evaluate(rt);
    if(rt.has_exception())
        return {};
    auto value_object = value.to_object(rt);
    if(rt.has_exception())
        return {};
    auto result = value_object->operator_subscript(rt, subscript_value);
    result.set_container(value_object);
    return result;
}

EvalResult NewExpression::evaluate(Runtime& rt) const
{
    Value name = m_name->evaluate(rt);
    if(rt.has_exception())
        return {};

    auto name_object = name.to_object(rt);
    if(rt.has_exception())
        return {};

    static StringId construct_name = "construct";
    Value construct_function = name_object->get(construct_name);
    if(rt.has_exception())
        return {};

    std::vector<Value> args;
    for(auto& arg: m_arguments)
    {
        args.push_back(arg->evaluate(rt));
        if(rt.has_exception())
            return {};
    }

    construct_function.set_container(name_object);
    return construct_function.call(rt, args);
}

EvalResult UnaryExpression::evaluate(Runtime& rt) const
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
        case PostfixIncrement:
            result = abstract::postfix_increment(rt, value);
            break;
        case PostfixDecrement:
            result = abstract::postfix_decrement(rt, value);
            break;
        default:
            assert(false);
    }

    return result;
}

EvalResult AssignmentExpression::evaluate(Runtime& rt) const
{
    Value lhs = m_lhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating LHS
    
    Value rhs = m_rhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating RHS

    auto reference = lhs.to_writable_reference(rt);
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

    lhs_value.assign(rt, result);
    if(rt.has_exception())
        return {};

    return Value::new_reference(reference);
}

EvalResult NormalBinaryExpression::evaluate(Runtime& rt) const
{
    Value lhs = m_lhs->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating LHS
    
    Value rhs = m_rhs->evaluate(rt);
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
        case Equal: 
            result = Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Equal);
            break;
        case NonEqual: 
            result = Value::new_bool(abstract::compare(rt, lhs, rhs) != CompareResult::Equal);
            break;
        case Greater: 
            result = Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Greater);
            break;
        case GreaterEqual:
        {
            auto compare = abstract::compare(rt, lhs, rhs);
            result = Value::new_bool(compare != CompareResult::Less && compare != CompareResult::Unknown);
            break;
        }
        case Less:
            result = Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Less);
            break;
        case LessEqual:
        {
            auto compare = abstract::compare(rt, lhs, rhs);
            result = Value::new_bool(compare != CompareResult::Greater && compare != CompareResult::Unknown);
            break;
        }
        case And:
        {
            auto lhs_bool = lhs.to_bool(rt);
            if(rt.has_exception())
                return {};
            // NOTE: C++ handles short-circuit.
            result = Value::new_bool(lhs_bool && rhs.to_bool(rt));
            break;
        }
        case Or:
        {
            auto lhs_bool = lhs.to_bool(rt);
            if(rt.has_exception())
                return {};
            // NOTE: C++ handles short-circuit.
            result = Value::new_bool(lhs_bool || rhs.to_bool(rt));
            break;
        }
        default:
            assert(false);
    }

    return result;
}

EvalResult FunctionExpression::evaluate(Runtime& rt) const
{
    return Value::new_object(std::make_shared<ASTFunction>(m_name, m_body, m_arg_names));
}

std::string FunctionExpression::to_string() const
{
    if(is_error())
        return ASTNode::to_string();

    return "FunctionExpression(" + name() + " {" + m_body->to_string() + "})";
}

EvalResult ExpressionStatement::evaluate(Runtime& rt) const
{
    return m_expression->evaluate(rt);
}

EvalResult BlockStatement::evaluate(Runtime& rt) const
{
    if(m_nodes.empty())
        return Value::new_bool(true);

    // TODO: Add some "inheritance" mechanism for block statement nodes
    Scope scope(rt);
    Value val = Value::undefined();
    for(auto& it: m_nodes)
    {
        auto result = it->evaluate(rt);
        if(rt.has_exception())
            return {};
        if(result.is_abrupt())
            return result;
        val = std::move(result);
    }
    return val;
}

EvalResult IfStatement::evaluate(Runtime& rt) const
{
    Value condition = m_condition->evaluate(rt);
    if(rt.has_exception())
        return {};
    
    auto condition_is_true = condition.to_bool(rt);
    if(condition_is_true)
        return m_true_statement->evaluate(rt);
    else
    {
        if(m_false_statement)
            return m_false_statement->evaluate(rt);
        return Value::undefined();
    }
}

EvalResult WhileStatement::evaluate(Runtime& rt) const
{
    Value result_value = Value::undefined();
    while(true)
    {
        Value condition = m_condition->evaluate(rt);
        if(rt.has_exception())
            return {};

        auto condition_is_true = condition.to_bool(rt);
        if(!condition_is_true)
            break;

        auto result = m_statement->evaluate(rt);
        if(rt.has_exception())
            return {};
        if(result.is_abrupt() && !result.is_continue())
            return result.value();
        result_value = result.value();
    }
    return result_value;
}

EvalResult ForStatement::evaluate(Runtime& rt) const
{
    Value result_value = Value::undefined();
    Scope scope(rt);
    if(m_initialization)
    {
        m_initialization->evaluate(rt);
        if(rt.has_exception())
            return {};
    }
    while(true)
    {
        auto condition = m_condition ? m_condition->evaluate(rt).value() : Value::new_bool(true);
        if(rt.has_exception())
            return {};

        auto condition_is_true = condition.to_bool(rt);
        if(!condition_is_true)
            break;

        auto result = m_statement->evaluate(rt);
        if(rt.has_exception())
            return {};
        if(result.is_abrupt() && !result.is_continue())
            return result.value();
        result_value = result.value();

        if(m_incrementation)
        {
            m_incrementation->evaluate(rt);
            if(rt.has_exception())
                return {};
        }
    }
    return result_value;
}

EvalResult ReturnStatement::evaluate(Runtime& rt) const
{
    if(!m_expression)
        return EvalResult::return_(Value::undefined());
    auto result = m_expression->evaluate(rt);
    if(result.is_abrupt())
        return result;
    return EvalResult::return_(result);
}

EvalResult SimpleControlStatement::evaluate(Runtime&) const
{
    switch(m_operation)
    {
        case Break:
            return EvalResult::break_(Value::undefined());
        case Continue:
            return EvalResult::continue_(Value::undefined());
        default:
            assert(false);
            return {};
    }
}

EvalResult VariableDeclaration::evaluate(Runtime& rt) const
{
    auto scope = rt.scope_object();
    assert(scope);
    Value init_value;
    if(m_initializer)
    {
        init_value = m_initializer->evaluate(rt);
        if(rt.has_exception())
            return {};
    }
    // TODO: Handle redefinition
    auto memory_value = scope->allocate(m_name);
    if(m_initializer)
        memory_value->value() = init_value;
    if(m_type == Const)
        memory_value->set_read_only(true);
    return Value::new_reference(memory_value);
}

EvalResult FunctionDeclaration::evaluate(Runtime& rt) const
{
    // TODO: Don't allow redefinition in the same scope
    auto result = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {};

    auto scope = rt.scope_object();
    assert(scope);
    // TODO: Handle redefinition
    scope->allocate(m_expression->name())->value().assign(rt, result);
    return result;
}

EvalResult Program::evaluate(Runtime& rt) const
{
    Value val;
    for(auto& it: m_nodes)
    {
        auto result = it->evaluate(rt);
        if(rt.has_exception())
            return {};
        if(result.is_return())
        {
            rt.throw_exception("Cannot 'return' in global scope");
            return {};
        }
        val = std::move(result);
    }
    return val;
}

std::ostream& operator<<(std::ostream& stream, ASTNode& node)
{
    return stream << (node.is_error() ? "" : node.to_string());
}

}
