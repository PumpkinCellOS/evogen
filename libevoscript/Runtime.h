#pragma once

#include <libevoscript/ExecutionContext.h>
#include <libevoscript/Value.h>
#include <libevoscript/objects/GlobalObject.h>

#include <stack>
#include <string>

namespace evo::script
{

class Runtime
{
public:
    Runtime(std::shared_ptr<GlobalObject> global_object = nullptr, std::shared_ptr<MemoryValue> global_this = nullptr);
    ~Runtime();

    void throw_exception(std::string const& message);
    void clear_exception() { m_exception_message = ""; }

    bool has_exception() const { return !m_exception_message.empty(); }
    std::string exception_message() const { return m_exception_message; }

    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> this_object);
    ExecutionContext& push_scope();
    ExecutionContext& current_execution_context();
    void pop_execution_context();

    void print_backtrace();

    template<class T = Object>
    std::shared_ptr<T> this_object() { return current_execution_context().this_object<T>(); }

    std::shared_ptr<LocalObject> local_scope_object() { return current_execution_context().local_scope_object(); }
    std::shared_ptr<GlobalObject> global_object() { return m_global_object; }

private:
    std::string m_exception_message;
    std::deque<ExecutionContext> m_execution_context_stack;
    std::shared_ptr<GlobalObject> m_global_object;
    std::shared_ptr<MemoryValue> m_global_this;
};

}
