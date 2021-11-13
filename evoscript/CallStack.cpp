#include <evoscript/CallStack.h>

#include <evoscript/EscapeSequences.h>

#include <iostream>

namespace evo::script
{

void CallStack::print(std::ostream& stream, bool detailed) const
{
    using namespace escapes;
    for(auto& it: m_call_stack)
    {
        if(detailed || !it.name().empty())
        {
            stream << "    at " << name(it.name()) << " ";
            if(detailed)
            {
                stream << ": ScopeObject[" << &it.scope_object() << "] parent=" << it.scope_object().parent() << " value=";
                it.scope_object().repl_print(stream, true);
            }
            stream << std::endl;
        }
    }
}

ExecutionContext& CallStack::push_execution_context(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object)
{
    return m_call_stack.emplace_front(name, this_object, global_object, *global_object.get());
}

ExecutionContext& CallStack::push_scope(std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object)
{
    return m_call_stack.emplace_front("", this_object, global_object, !m_call_stack.empty() ? current_execution_context().scope_object() : *global_object);
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
    assert(!m_call_stack.empty());
    m_call_stack.pop_front();
}

}
