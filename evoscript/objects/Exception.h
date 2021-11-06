#pragma once

#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <memory>

namespace evo::script
{

class Exception : public Class, public NativeClass<Exception>
{
    struct InternalData : public ObjectInternalData
    {
        CallStack call_stack;
        std::string message;

        InternalData(CallStack const& call_stack_, std::string const& message_)
        : call_stack(call_stack_), message(message_) {}
    };
public:
    Exception();

    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, std::string const& message = "") const
    {
        assert(rt);
        return std::make_unique<InternalData>(rt->call_stack(), message);
    }

    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, ArgumentList const& args) const override
    {
        return construct_internal_data(rt, args.get(0).to_string());
    }

    virtual void print(Object const&, std::ostream&, bool print_members, bool dump) const override;

private:
    Value print_(Runtime& rt, ArgumentList const&);
};

}
