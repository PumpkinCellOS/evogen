#pragma once

#include <libevoscript/Value.h>
#include <libevoscript/objects/LocalObject.h>

namespace evo::script
{

class ExecutionContext
{
public:
    ExecutionContext(std::shared_ptr<Object> this_object, std::shared_ptr<LocalObject> parent_scope);

    template<class T = Object>
    std::shared_ptr<T> this_object() const { return std::static_pointer_cast<T>(m_this); }

    std::shared_ptr<LocalObject> local_scope_object() const { return m_local_scope; }

private:
    std::shared_ptr<Object> m_this;
    std::shared_ptr<LocalObject> m_local_scope;
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

class Scope
{
public:
    Scope(Runtime& rt);
    ~Scope();

    ExecutionContext& context() { return m_context; }

private:
    Runtime& m_rt;
    ExecutionContext& m_context;
};

}
