#pragma once

#include <libevoscript/Value.h>

namespace evo::script
{

class ExecutionContext
{
public:
    ExecutionContext(std::shared_ptr<Object> this_object);

    template<class T = Object>
    std::shared_ptr<T> this_object() const { return std::static_pointer_cast<T>(m_this); }

    std::shared_ptr<MemoryValue> local_scope_object() const { return m_local_scope; }

private:
    std::shared_ptr<Object> m_this;
    std::shared_ptr<MemoryValue> m_local_scope;
};

class ScopedExecutionContext
{
public:
    ScopedExecutionContext(Runtime& rt, std::shared_ptr<Object> this_object);
    ~ScopedExecutionContext();

    ExecutionContext& context() { return m_context; }

private:
    Runtime& m_rt;
    ExecutionContext& m_context;
};

}
