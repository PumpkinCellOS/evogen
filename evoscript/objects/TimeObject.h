#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class TimeObject : public Object
{
public:
    explicit TimeObject(Value::IntType value);
    TimeObject(Runtime&, std::vector<Value> const& args);

    EVO_OBJECT("Time")

    virtual Value to_primitive(Runtime&, Value::Type) const override;

private:
    Value::IntType m_value;
};

}
