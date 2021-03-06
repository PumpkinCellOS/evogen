#include <evoscript/AST.h>

#include <evoscript/AbstractOperations.h>
#include <evoscript/EvalResult.h>
#include <evoscript/ExecutionContext.h>
#include <evoscript/Object.h>
#include <evoscript/objects/ASTFunction.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/NativeFunction.h>
#include <evoscript/objects/String.h>

/*
#include <evoscript/objects/ASTFunction.h>
#include <evoscript/objects/StringObject.h>
*/

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>

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

EvalResult StringLiteral::evaluate(Runtime& rt) const
{
    return Value::new_object(Object::create_native<String>(&rt, m_value));
}

EvalResult Identifier::evaluate(Runtime& rt) const
{
    auto [scope, reference] = rt.resolve_identifier(m_name);
    if(!reference)
    {
        rt.throw_exception<Exception>("'" + m_name.string() + "' is not declared");
        return {};
    }

    return Value::new_reference(reference, scope);
}

EvalResult SpecialValue::evaluate(Runtime& rt) const
{
    switch(m_type)
    {
    case This:
    {
        auto this_ = rt.resolve_this_object();
        if(!this_)
            return {};
        return Value::new_reference(MemoryValue::create_object(this_), nullptr);
    }
    case Global:
        return Value::new_reference(MemoryValue::create_object(rt.global_object()), nullptr);
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
    if(!member) // get() failed (e.g nonexisting objects are not tolerated)
    {
        rt.throw_exception<Exception>("Member '" + object->to_string() + "." + m_name.string() + "' is not declared");
        return {};
    }

    auto member_value = Value::new_reference(member, object.get());
    return member_value;
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

    return callable.call(rt, ArgumentList{arguments});
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
    return value_object->operator_subscript(rt, subscript_value);
}

EvalResult NewExpression::evaluate(Runtime& rt) const
{
    Value name = m_name->evaluate(rt);
    if(rt.has_exception())
        return {};

    auto name_object = name.to_object(rt);
    if(rt.has_exception())
        return {};

    static StringId construct_sid = "__construct";
    auto construct_function = Value::new_reference(name_object->get(construct_sid), name_object.get());
    if(rt.has_exception())
        return {};

    std::vector<Value> args;
    for(auto& arg: m_arguments)
    {
        args.push_back(arg->evaluate(rt));
        if(rt.has_exception())
            return {};
    }

    return construct_function.call(rt, ArgumentList{args});
}

EvalResult UnaryExpression::evaluate(Runtime& rt) const
{
    auto value = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {}; // Error evaluating expression

    switch(m_operation)
    {
        case Not:
            return abstract::not_(rt, value);
            break;
        case BitwiseNot:
            return abstract::bitwise_not(rt, value);
            break;
        case Minus:
            return abstract::minus(rt, value);
            break;
        case Plus:
            return abstract::plus(rt, value);
            break;
        case Increment:
            return abstract::prefix_increment(rt, value);
            break;
        case Decrement:
            return abstract::prefix_decrement(rt, value);
            break;
        case PostfixIncrement:
            return abstract::postfix_increment(rt, value);
            break;
        case PostfixDecrement:
            return abstract::postfix_decrement(rt, value);
            break;
        default:
            abort();
    }
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

    return Value::new_reference(reference, lhs.container());
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
            return abstract::add(rt, lhs, rhs);
        case Subtract:
            return abstract::subtract(rt, lhs, rhs);
        case Multiply:
            return abstract::multiply(rt, lhs, rhs);
        case Divide:
            return abstract::divide(rt, lhs, rhs);
        case Modulo:
            return abstract::modulo(rt, lhs, rhs);
        case Equal: 
            return Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Equal);
        case NonEqual: 
            return Value::new_bool(abstract::compare(rt, lhs, rhs) != CompareResult::Equal);
        case Greater: 
            return Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Greater);
        case GreaterEqual:
        {
            auto compare = abstract::compare(rt, lhs, rhs);
            return Value::new_bool(compare != CompareResult::Less && compare != CompareResult::Unknown);
        }
        case Less:
            return Value::new_bool(abstract::compare(rt, lhs, rhs) == CompareResult::Less);
        case LessEqual:
        {
            auto compare = abstract::compare(rt, lhs, rhs);
            return Value::new_bool(compare != CompareResult::Greater && compare != CompareResult::Unknown);
        }
        case And:
        {
            auto lhs_bool = lhs.to_bool(rt);
            if(rt.has_exception())
                return {};
            // NOTE: C++ handles short-circuit.
            return Value::new_bool(lhs_bool && rhs.to_bool(rt));
        }
        case Or:
        {
            auto lhs_bool = lhs.to_bool(rt);
            if(rt.has_exception())
                return {};
            // NOTE: C++ handles short-circuit.
            return Value::new_bool(lhs_bool || rhs.to_bool(rt));
        }
        default:
            abort();
    }
}

EvalResult FunctionExpression::evaluate(Runtime& rt) const
{
    return Value::new_object(Object::create_native<ASTFunction>(&rt, m_name, m_body, m_arg_names));
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

EvalResult BlockStatement::evaluate_starting_from(Runtime& rt, size_t index) const
{
    if(m_nodes.empty())
        // HACK to make for loop working
        return Value::new_bool(true);

    // TODO: Add some "inheritance" mechanism for block statement nodes
    EvalResult result = Value::undefined();
    auto execute = [this, index, &rt]() {
        EvalResult result = Value::undefined();
        for(size_t s = index; s < m_nodes.size(); s++)
        {
            auto& statement = m_nodes[s];
            result = statement->evaluate(rt);
            if(result.is_abrupt())
                return result;
        }
        assert(result.is_normal());
        return result;
    };

    if(needs_scope())
    {
        Scope scope(rt);
        result = execute();
    }
    else
        result = execute();
    if(rt.has_exception())
        return {};
    return result;
}

EvalResult BlockStatement::evaluate(Runtime& rt) const
{
    return evaluate_starting_from(rt, 0);
}

bool CaseLabel::matches(Runtime& rt, Value const& value) const
{
    return abstract::compare(rt, m_literal->evaluate(rt).value(), value) == CompareResult::Equal;
}

std::optional<size_t> BlockStatement::find_matching_case_label(Runtime& rt, Value const& value)
{
    for(auto& it: m_case_labels)
    {
        if(it.first->matches(rt, value))
            return it.second;
    }
    return m_default_label;
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

EvalResult SwitchStatement::evaluate(Runtime& rt) const
{
    auto check_value = m_expression->evaluate(rt);
    if(check_value.is_abrupt() || rt.has_exception())
        return check_value;

    auto case_label = m_statement->find_matching_case_label(rt, check_value);
    if(!case_label.has_value())
        return Value::undefined();

    return m_statement->evaluate_starting_from(rt, case_label.value());
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

EvalResult ThrowStatement::evaluate(Runtime& rt) const
{
    auto result = m_expression->evaluate(rt);
    if(!result.value().is_object())
    {
        rt.throw_exception<Exception>("'throw' argument must be an object");
        return {};
    }
    rt.throw_exception(result.value().get_object());
    return result;
}

EvalResult TryCatchStatement::evaluate(Runtime& rt) const
{
    auto result = m_try_statement->evaluate(rt);
    if(rt.has_exception())
    {
        Scope scope(rt);
        auto catch_variable = rt.scope_object().allocate(m_catch_variable);
        catch_variable->value() = Value::new_object(rt.exception());
        rt.clear_exception();
        return m_catch_statement->evaluate(rt);
    }
    return result;
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
    auto& scope = rt.scope_object();
    Value init_value;
    if(m_initializer)
    {
        init_value = m_initializer->evaluate(rt);
        if(rt.has_exception())
            return {};
    }
    // TODO: Handle redefinition
    auto memory_value = scope.allocate(m_name);
    if(m_initializer)
        memory_value->value() = init_value;
    if(m_type == Const)
        memory_value->set_read_only(true);
    return Value::new_reference(memory_value, &scope);
}

EvalResult FunctionDeclaration::evaluate(Runtime& rt) const
{
    // TODO: Don't allow redefinition in the same scope
    auto result = m_expression->evaluate(rt);
    if(rt.has_exception())
        return {};

    auto& scope = rt.scope_object();
    // TODO: Handle redefinition
    scope.allocate(m_expression->name())->value().assign(rt, result);
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
            rt.throw_exception<Exception>("Cannot 'return' in global scope");
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
