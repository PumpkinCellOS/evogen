#pragma once

#include <evoscript/CallStack.h>
#include <evoscript/ExecutionContext.h>
#include <evoscript/StringId.h>
#include <evoscript/Value.h>
#include <evoscript/objects/GlobalObject.h>

#include <stack>
#include <string>

namespace evo::script
{

class Exception;

class Runtime
{
public:
    Runtime(std::shared_ptr<GlobalObject> const& global_object = nullptr, std::shared_ptr<MemoryValue> const& global_this = nullptr);
    ~Runtime();

    void throw_exception(std::string const& message);
    void clear_exception() { m_exception.reset(); }

    bool has_exception() const { return !!m_exception; }
    std::shared_ptr<Exception> exception() const { return m_exception; }

    CallStack const& call_stack() const { return m_call_stack; }
    CallStack& call_stack() { return m_call_stack; }

    template<class T = Object>
    std::shared_ptr<T> this_object() { return call_stack().current_execution_context().this_object<T>(); }

    std::shared_ptr<LocalObject> local_scope_object() { return call_stack().current_execution_context().local_scope_object(); }
    std::shared_ptr<GlobalObject> global_object() { return m_global_object; }
    void print_backtrace() const;

    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object);
    ExecutionContext& push_scope();
    ExecutionContext& current_execution_context();
    void pop_execution_context();

    enum class RunType
    {
        Repl,
        Script,
        Include
    };

    Value run_code_from_stream(std::istream&, RunType);

private:
    std::shared_ptr<Exception> m_exception;
    CallStack m_call_stack;
    std::shared_ptr<GlobalObject> m_global_object;
    std::shared_ptr<MemoryValue> m_global_this;
};

}
