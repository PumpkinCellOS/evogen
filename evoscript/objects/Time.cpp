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

void Time::constructor(Runtime&, NativeObject<Time>& object, Value::IntType value) const
{
    object.internal_data().milliseconds = value;
}

void Time::constructor(Runtime& rt, NativeObject<Time>& object, ArgumentList const& args) const
{
    if(!args.is_given(0))
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        constructor(rt, object, ts.tv_sec * 1000 + ts.tv_nsec / 1'000'000);
    }
    auto arg_int = args.get(0).to_int(rt);
    if(rt.has_exception())
        return;
    constructor(rt, object, arg_int);
}

}
