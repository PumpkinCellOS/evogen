#include <libevoscript/ExecutionContext.h>

#include <libevoscript/Runtime.h>
#include <libevoscript/objects/LocalObject.h>
#include <libevoscript/objects/Object.h>

namespace evo::script
{

ExecutionContext::ExecutionContext(std::shared_ptr<Object> this_object)
{
    assert(this_object);
    m_this = this_object;
    m_local_scope = MemoryValue::create_object<LocalObject>();
}

ScopedExecutionContext::ScopedExecutionContext(Runtime& rt, std::shared_ptr<Object> this_object)
: m_rt(rt), m_context(rt.push_execution_context(this_object)) {}

ScopedExecutionContext::~ScopedExecutionContext()
{
    m_rt.pop_execution_context();
}

}
