#pragma once

#include <libevoscript/ExecutionContext.h>
#include <libevoscript/Value.h>

#include <stack>
#include <string>

namespace evo::script
{

class Runtime
{
public:
    Runtime(std::shared_ptr<MemoryValue> global_this = nullptr);
    ~Runtime();

    void throw_exception(std::string const& message);
    void clear_exception() { m_exception_message = ""; }

    bool has_exception() const { return !m_exception_message.empty(); }
    std::string exception_message() const { return m_exception_message; }

    ExecutionContext& push_execution_context(std::shared_ptr<Object> this_object);
    ExecutionContext& current_execution_context();
    void pop_execution_context();

private:
    std::string m_exception_message;
    std::stack<ExecutionContext> m_execution_context_stack;
    std::shared_ptr<MemoryValue> m_global_this;
};

}
