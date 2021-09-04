#include <libevoscript/ExecutionContext.h>

#include <libevoscript/Runtime.h>
#include <libevoscript/objects/LocalObject.h>
#include <libevoscript/objects/Object.h>

namespace evo::script
{

ExecutionContext::ExecutionContext(std::shared_ptr<Object> this_object, std::shared_ptr<LocalObject> parent_scope)
{
    assert(this_object);
    m_this = this_object;
    m_local_scope = std::make_shared<LocalObject>(parent_scope);
}

ScopedExecutionContext::ScopedExecutionContext(Runtime& rt, std::shared_ptr<Object> this_object)
: m_rt(rt), m_context(rt.push_execution_context(this_object)) {}

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
