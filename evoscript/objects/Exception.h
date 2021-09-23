#pragma once

#include <evoscript/objects/Object.h>

#include <evoscript/Runtime.h>

namespace evo::script
{

class Exception : public Object
{
public:
    Exception(Runtime& rt, std::string const& message);
    Exception(Runtime& rt, std::vector<Value> const& args)
    : Exception(rt, args.size() > 0 ? args[0].to_string() : "") {}

    EVO_OBJECT("Exception")

    virtual void repl_print(std::ostream& output, bool) const override;

private:
    Value print_(Runtime& rt, std::vector<Value> const&);

    CallStack m_call_stack;
    std::string m_message;
};

}
