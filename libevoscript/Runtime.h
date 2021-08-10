#pragma once

#include <libevoscript/Value.h>

#include <string>

namespace evo::script
{

class Runtime
{
public:
    Runtime();
    ~Runtime();

    void throw_exception(std::string const& message);
    void clear_exception() { m_exception_message = ""; }

    // TODO: Move this to some Scope
    std::shared_ptr<MemoryValue> this_object() const { return m_this; }
    std::shared_ptr<MemoryValue> local_scope_object() const { return m_local_scope; }

    bool has_exception() const { return !m_exception_message.empty(); }
    std::string exception_message() const { return m_exception_message; }

private:
    // TODO: Move this to some Scope
    std::shared_ptr<MemoryValue> m_this;
    std::shared_ptr<MemoryValue> m_local_scope;
    std::string m_exception_message;
};

}
