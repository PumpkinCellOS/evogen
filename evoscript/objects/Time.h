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

    void constructor(Runtime&, NativeObject<Time>&, Value::IntType) const;
    virtual void constructor(Runtime&, NativeObject<Time>&, ArgumentList const&) const override;
};

}
