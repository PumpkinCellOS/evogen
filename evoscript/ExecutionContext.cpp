#include <evoscript/ExecutionContext.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/ScopeObject.h>

namespace evo::script
{

ExecutionContext::ExecutionContext(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object, std::shared_ptr<ScopeObject> const& scope)
:  m_this(this_object), m_local_scope(scope), m_global_object(global_object), m_name(name)
{
    assert(this_object);
}

ScopedExecutionContext::ScopedExecutionContext(Runtime& rt, std::string const& name, std::shared_ptr<Object> const& this_object)
: m_rt(rt), m_context(rt.push_execution_context(name, this_object)) {}

ScopedExecutionContext::~ScopedExecutionContext()
{
    m_rt.pop_execution_context();
}

Scope::Scope(Runtime& rt)
: m_rt(rt), m_context(rt.push_scope()) {}

Scope::~Scope()
{
    m_rt.pop_execution_context();
}

}
