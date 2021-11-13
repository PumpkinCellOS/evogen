#pragma once

#include <evoscript/ExecutionContext.h>

#include <deque>

namespace evo::script
{

class CallStack
{
public:
    CallStack() = default;
    CallStack(CallStack const& other) = delete;

    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object);
    ExecutionContext& push_scope(std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object);
    ExecutionContext const& current_execution_context() const;
    ExecutionContext& current_execution_context();
    void pop_execution_context();
    void print(std::ostream& stream, bool detailed = false) const;

    template<class Callback> requires requires(Callback&& callback, ExecutionContext const& ctx) { callback(ctx); }
    void walk_stacktrace(Callback&& callback) const
    {
        for(auto& it: m_call_stack)
        {
            if(!it.name().empty())
                callback(it);
        }
    }

    bool is_empty() const { return m_call_stack.empty(); }

private:
    std::deque<ExecutionContext> m_call_stack;
};

}
