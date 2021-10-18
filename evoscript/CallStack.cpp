#include <evoscript/CallStack.h>

#include <evoscript/EscapeSequences.h>

#include <iostream>

namespace evo::script
{

void CallStack::print(std::ostream& stream) const
{
    using namespace escapes;
    for(auto& it: m_call_stack)
    {
        if(!it.name().empty())
            stream << "    at " << name(it.name()) << std::endl;
    }
}

ExecutionContext& CallStack::push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object)
{
    return m_call_stack.emplace_front(name, this_object, global_object);
}

ExecutionContext& CallStack::push_scope(std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object)
{
    return m_call_stack.emplace_front("", this_object, global_object, current_execution_context().scope_object());
}

ExecutionContext const& CallStack::current_execution_context() const
{
    return m_call_stack.front();
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
