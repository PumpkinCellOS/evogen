#pragma once

#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <memory>

namespace evo::script
{

class Exception : public Class, public NativeClass<Exception>
{
public:
    struct InternalData : public ObjectInternalData
    {
        std::vector<std::string> stack_frames;
        std::string message;

        InternalData(CallStack const& call_stack_, std::string const& message_);
    };

    Exception();

    void constructor(Runtime& rt, NativeObject<Exception>& object, std::string const& message = "") const
    {
        object.internal_data() = InternalData{rt.call_stack(), message};
    }

    void constructor(Runtime& rt, NativeObject<Exception>& object, ArgumentList const& args) const
    {
        constructor(rt, object, args.get(0).to_string());
    }

private:
    static Value print_(Runtime& rt, ArgumentList const&);
};

}
