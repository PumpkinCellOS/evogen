#include <evoscript/CallStack.h>

#include <iostream>

namespace evo::script
{

void CallStack::print() const
{
    for(auto& it: m_call_stack)
    {
        if(!it.parent_scope())
            std::cerr << "    at " << it.name() << std::endl;
    }
}

ExecutionContext& CallStack::push_execution_context(std::string const& name, std::shared_ptr<Object> this_object)
{
    return m_call_stack.emplace_front(name, this_object, nullptr);
}

ExecutionContext& CallStack::push_scope(std::shared_ptr<Object> this_object)
{
    return m_call_stack.emplace_front("", this_object, current_execution_context().local_scope_object());
}

ExecutionContext& CallStack::current_execution_context()
{
    return m_call_stack.front();
}

void CallStack::pop_execution_context()
{
    m_call_stack.pop_front();
}

}
