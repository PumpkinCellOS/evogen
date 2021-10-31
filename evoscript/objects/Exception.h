#pragma once

#include <evoscript/objects/Object.h>

#include <evoscript/Runtime.h>

namespace evo::script
{

class Exception : public Object
{
public:
    Exception(Runtime& rt, std::string const& message);
    Exception(Runtime& rt, ArgumentList const& args)
    : Exception(rt, args.get(0).to_string()) {}

    EVO_OBJECT("Exception")

    virtual void repl_print(std::ostream& output, bool) const override;

    std::string message() const { return m_message; }

private:
    Value print_(Runtime& rt, ArgumentList const&);

    CallStack m_call_stack;
    std::string m_message;
};

}
