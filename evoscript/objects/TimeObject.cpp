#include <evoscript/objects/TimeObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>

namespace evo::script
{

TimeObject::TimeObject(Value::IntType milliseconds)
: m_milliseconds(milliseconds)
{
    // TODO: Change it to a variable when getters are implemented
    static StringId milliseconds_sid = "milliseconds";
    define_native_function<TimeObject>(milliseconds_sid, [](TimeObject* object, Runtime&, ArgumentList const&) {
        return Value::new_int(object->m_milliseconds);
    });
}

TimeObject::TimeObject(Runtime& rt, ArgumentList const& args)
: TimeObject(0)
{
    if(args.size() != 1)
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        m_milliseconds = ts.tv_sec * 1000 + ts.tv_nsec / 1'000'000;
    }
    else
    {
        m_milliseconds = args.get(0).to_int(rt);
        if(rt.has_exception())
            return;
    }
}

Value TimeObject::to_primitive(Runtime& rt, Value::Type type) const
{
    if(type == Value::Type::Int)
        return Value::new_int(m_milliseconds);
    rt.throw_exception("Cannot convert Time to primitive of type " + Value::type_to_string(type));
    return {};
}

}
