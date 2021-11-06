#pragma once

#include <evoscript/Object.h>

namespace evo::script
{

class Time : public Class, public NativeClass<Time>
{
    struct InternalData : public ObjectInternalData
    {
        Value::IntType milliseconds;

        InternalData(Value::IntType milliseconds_)
        : milliseconds(milliseconds_) {}
    };
public:
    Time();

    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, Value::IntType) const;
    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, ArgumentList const&) const override;

    virtual Value to_primitive(Runtime&, Object const&, Value::Type) const override;
};

}
