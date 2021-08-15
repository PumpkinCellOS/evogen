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

}
