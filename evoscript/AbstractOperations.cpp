#include <evoscript/AbstractOperations.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/StringObject.h>

namespace evo::script::abstract
{

Value add(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    
    auto lhs_int = real_lhs.to_int(rt);
    auto rhs_int = real_rhs.to_int(rt);

    // Both arguments are convertible to ints
    if(!rt.has_exception())
        return Value::new_int(lhs_int + rhs_int);

    // LHS is object
    rt.clear_exception();
    if(real_lhs.is_object())
    {
        return real_lhs.get_object()->operator_add(rt, real_rhs);
    }
    else
    {
        // Unknown case, treat as strings.
        auto lhs_int = real_lhs.to_string();
        auto rhs_int = real_rhs.to_string();
        return new_object_value<StringObject>(lhs_int + rhs_int);
    }
}

Value subtract(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    auto lhs_int = real_lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = real_rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(lhs_int - rhs_int);
}

Value multiply(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    auto lhs_int = real_lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = real_rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(lhs_int * rhs_int);
}

Value divide(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    auto lhs_int = real_lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = real_rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(rhs_int == 0)
    {
        rt.throw_exception("Cannot divide by 0");
        return {};
    }
    return Value::new_int(lhs_int / rhs_int);
}

Value modulo(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    auto lhs_int = real_lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = real_rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(rhs_int == 0)
    {
        rt.throw_exception("Cannot modulo by 0");
        return {};
    }
    return Value::new_int(lhs_int % rhs_int);
}

Value not_(Runtime& rt, Value const& value)
{
    auto real_value = value.dereferenced();
    auto value_bool = real_value.to_bool(rt);
    if(rt.has_exception())
        return {};

    return Value::new_bool(!value_bool);
}

Value bitwise_not(Runtime& rt, Value const& value)
{
    auto real_value = value.dereferenced();
    auto value_int = real_value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(~value_int);
}

Value minus(Runtime& rt, Value const& value)
{
    auto real_value = value.dereferenced();
    auto value_int = real_value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(-value_int);
}

Value plus(Runtime& rt, Value const& value)
{
    auto real_value = value.dereferenced();
    auto value_int = real_value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(+value_int);
}

template<class T>
static CompareResult compare_primitive(T const& l, T const& r)
{
    return l == r ? CompareResult::Equal : (l < r ? CompareResult::Less : CompareResult::Greater);
}

CompareResult compare(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto real_lhs = lhs.dereferenced();
    auto real_rhs = rhs.dereferenced();
    if(real_lhs.type() != real_rhs.type())
    {
        rt.throw_exception("Cannot compare values of different type (For now)");
        return CompareResult::Unknown;
    }
    if(real_lhs.is_null() || real_rhs.is_null() || real_lhs.is_undefined() || real_rhs.is_undefined())
        return CompareResult::Unknown;
    
    if(real_lhs.is_int())
        return compare_primitive(real_lhs.get_int(), real_rhs.get_int());
    else if(real_lhs.is_bool())
        return real_lhs.get_bool() == real_rhs.get_bool() ? CompareResult::Equal : CompareResult::Unknown;
    else if(real_lhs.is_object())
        return real_lhs.get_object()->operator_compare(rt, real_rhs);
    else
    {
        // TODO: Object compare
        rt.throw_exception("Cannot compare values of type " + real_lhs.type_string());
        return CompareResult::Unknown;
    }
}

Value postfix_increment(Runtime& rt, Value const& value)
{
    auto old_value = value.dereferenced();
    prefix_increment(rt, value);
    return old_value;
}

Value postfix_decrement(Runtime& rt, Value const& value)
{
    auto old_value = value.dereferenced();
    prefix_decrement(rt, value);
    return old_value;
}

Value prefix_increment(Runtime& rt, Value const& value)
{
    auto reference = value.to_writable_reference(rt);
    if(rt.has_exception())
        return {};

    auto value_int = reference->value().to_int(rt);
    if(rt.has_exception())
        return {};

    reference->value().assign(rt, Value::new_int(value_int + 1));
    if(rt.has_exception())
        return {};

    return value;
}

Value prefix_decrement(Runtime& rt, Value const& value)
{
    auto reference = value.to_writable_reference(rt);
    if(rt.has_exception())
        return {};

    auto value_int = reference->value().to_int(rt);
    if(rt.has_exception())
        return {};

    reference->value().assign(rt, Value::new_int(value_int - 1));
    if(rt.has_exception())
        return {};

    return value;
}

}
