#pragma once

#include <evoscript/objects/Object.h>

#include <evoscript/Runtime.h>

namespace evo::script
{

class Exception : public Object
{
public:
    Exception(Runtime& rt, std::string const& message);

    virtual void print();
    virtual std::string type_name() const override { return "Exception"; }

private:
    static Value print_(Runtime& rt, Exception& container, std::vector<Value> const&);

    CallStack m_call_stack;
    std::string m_message;
};

}
