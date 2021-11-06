#include <evoscript/objects/Time.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

Time::Time()
: Class("Time")
{
    define_native_function<Time>("milliseconds", [](Runtime&, Object& object, ArgumentList const&) {
        return Value::new_int(object.internal_data<InternalData>().milliseconds);
    });
}

std::unique_ptr<ObjectInternalData> Time::construct_internal_data(Runtime*, Value::IntType val) const
{
    return std::make_unique<InternalData>(val);
}

std::unique_ptr<ObjectInternalData> Time::construct_internal_data(Runtime* rt, ArgumentList const& args) const
{
    assert(rt);
    if(!args.is_given(0))
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return construct_internal_data(rt, ts.tv_sec * 1000 + ts.tv_nsec / 1'000'000);
    }
    auto arg_int = args.get(0).to_int(*rt);
    if(rt->has_exception())
        return nullptr;
    return construct_internal_data(rt, arg_int);
}

Value Time::to_primitive(Runtime& rt, Object const& object, Value::Type type) const
{
    if(type == Value::Type::Int)
        return Value::new_int(object.internal_data<InternalData>().milliseconds);
    rt.throw_exception<Exception>("Cannot convert Time to primitive of type " + Value::type_to_string(type));
    return {};
}

}
