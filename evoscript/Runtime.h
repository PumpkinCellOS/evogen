#pragma once

#include <evoscript/CallStack.h>
#include <evoscript/ExecutionContext.h>
#include <evoscript/GlobalObject.h>
#include <evoscript/Lexer.h>
#include <evoscript/StringId.h>
#include <evoscript/Value.h>

#include <stack>
#include <string>
#include <iostream>

namespace evo::script
{

class Exception;

class Runtime
{
public:
    Runtime(std::shared_ptr<GlobalObject> const& global_object = nullptr, std::shared_ptr<MemoryValue> const& global_this = nullptr);

    template<class T, class... Args>
    void throw_exception(Args&&... args)
    {
        m_exception = Object::create_native<T>(*this, std::forward<Args>(args)...);
    }

    void throw_exception(std::shared_ptr<Object> const& object) { m_exception = object; }
    void clear_exception() { m_exception.reset(); }
    bool has_exception() const { return !!m_exception; }
    std::shared_ptr<Object> exception() const { return m_exception; }

    CallStack const& call_stack() const { return m_call_stack; }
    CallStack& call_stack() { return m_call_stack; }

    std::shared_ptr<Object> this_object() const;
    std::shared_ptr<Object> resolve_this_object();

    void set_this_object(std::shared_ptr<MemoryValue> const& this_) { m_global_this = this_; }

    ScopeObject const& scope_object() const
    {
        return call_stack().is_empty() ? *global_object() : call_stack().current_execution_context().scope_object();
    }
    ScopeObject& scope_object()
    {
        return call_stack().is_empty() ? *global_object() : call_stack().current_execution_context().scope_object();
    }
    std::shared_ptr<GlobalObject> global_object() const { return m_global_object; }
    void print_backtrace() const;

    ScopeObject::IdentifierRecord resolve_identifier(StringId);

    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object);
    ExecutionContext& push_global_scope();
    ExecutionContext& push_scope();
    ExecutionContext& current_execution_context();
    ExecutionContext const& current_execution_context() const;
    void pop_execution_context();

    enum class RunType
    {
        Repl,
        Script,
        Include
    };

    Value run_code_from_stream(std::istream&, RunType);

    void set_output_stream(std::ostream& stream) { m_output_stream = &stream; }
    void set_error_stream(std::ostream& stream) { m_error_stream = &stream; }

    std::ostream& output_stream() const { return *m_output_stream; }
    std::ostream& error_stream() const { return *m_error_stream; }

private:
    std::shared_ptr<Object> m_exception;
    CallStack m_call_stack;
    std::shared_ptr<GlobalObject> m_global_object;
    std::shared_ptr<MemoryValue> m_global_this;
    std::ostream* m_output_stream = &std::cout;
    std::ostream* m_error_stream = &std::cerr;
};

}
