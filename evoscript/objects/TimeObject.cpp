#include "evoscript/NativeFunction.h"
#include <evoscript/objects/TimeObject.h>

#include <evoscript/Runtime.h>

namespace evo::script
{

TimeObject::TimeObject(Value::IntType value)
: m_value(value)
{
    // TODO: Change it to a variable when getters are implemented
    DEFINE_NATIVE_FUNCTION(TimeObject, "value", [](Runtime&, TimeObject& object, std::vector<Value> const&) {
        return Value::new_int(object.m_value);
    });
}

TimeObject::TimeObject(Runtime& rt, std::vector<Value> const& args)
: TimeObject(0)
{
    if(args.size() != 1)
        m_value = time(nullptr);
    else
    {
        m_value = args[0].to_int(rt);
        if(rt.has_exception())
            return;
    }
}

Value TimeObject::to_primitive(Runtime& rt, Value::Type type) const
{
    if(type == Value::Type::Int)
        return Value::new_int(m_value);
    rt.throw_exception("Cannot convert Time to primitive of type " + Value::type_to_string(type));
    return {};
}

}
