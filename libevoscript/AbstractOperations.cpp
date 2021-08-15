#include <libevoscript/AbstractOperations.h>

namespace evo::script::abstract
{

Value add(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto lhs_int = lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(lhs_int + rhs_int);
}

Value subtract(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto lhs_int = lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(lhs_int - rhs_int);
}

Value multiply(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto lhs_int = lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(lhs_int * rhs_int);
}

Value divide(Runtime& rt, Value const& lhs, Value const& rhs)
{
    auto lhs_int = lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = rhs.to_int(rt);
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
    auto lhs_int = lhs.to_int(rt);
    if(rt.has_exception())
        return {};

    auto rhs_int = rhs.to_int(rt);
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
    auto value_int = value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(!value_int);
}

Value bitwise_not(Runtime& rt, Value const& value)
{
    auto value_int = value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(~value_int);
}

Value minus(Runtime& rt, Value const& value)
{
    auto value_int = value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(-value_int);
}

Value plus(Runtime& rt, Value const& value)
{
    auto value_int = value.to_int(rt);
    if(rt.has_exception())
        return {};

    return Value::new_int(+value_int);
}

Value prefix_increment(Runtime& rt, Value const& value)
{
    auto reference = value.to_reference(rt);
    if(rt.has_exception())
        return {};

    auto value_int = reference->value().to_int(rt);
    if(rt.has_exception())
        return {};

    reference->value().assign(Value::new_int(value_int + 1));
    return value;
}

Value prefix_decrement(Runtime& rt, Value const& value)
{
    auto reference = value.to_reference(rt);
    if(rt.has_exception())
        return {};

    auto value_int = reference->value().to_int(rt);
    if(rt.has_exception())
        return {};

    reference->value().assign(Value::new_int(value_int - 1));
    return value;
}

}
