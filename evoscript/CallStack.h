#pragma once

#include <evoscript/ExecutionContext.h>

#include <deque>

namespace evo::script
{

class CallStack
{
public:
    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object);
    ExecutionContext& push_scope(std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object);
    ExecutionContext const& current_execution_context() const;
    ExecutionContext& current_execution_context();
    void pop_execution_context();
    void print(std::ostream& stream) const;

private:
    std::deque<ExecutionContext> m_call_stack;
};

}
