#pragma once

#include <libevoscript/ExecutionContext.h>

#include <deque>

namespace evo::script
{

class CallStack
{
public:
    ExecutionContext& push_execution_context(std::string const& name, std::shared_ptr<Object> this_object);
    ExecutionContext& push_scope(std::shared_ptr<Object> this_object);
    ExecutionContext& current_execution_context();
    void pop_execution_context();
    void print() const;

private:
    std::deque<ExecutionContext> m_call_stack;
};

}
