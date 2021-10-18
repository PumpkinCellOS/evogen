#pragma once

#include <evoscript/Value.h>
#include <evoscript/objects/ScopeObject.h>

namespace evo::script
{

class ExecutionContext
{
public:
    ExecutionContext(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<ScopeObject> const& parent_scope);

    template<class T = Object>
    std::shared_ptr<T> this_object() const { return std::static_pointer_cast<T>(m_this); }

    std::shared_ptr<ScopeObject> scope_object() const { return m_local_scope; }
    std::string name() const { return m_name; }
    std::shared_ptr<ScopeObject> parent_scope() const { return m_local_scope->parent(); }

private:
    std::shared_ptr<Object> m_this;
    std::shared_ptr<ScopeObject> m_local_scope;
    std::string m_name;
};

class ScopedExecutionContext
{
public:
    ScopedExecutionContext(Runtime& rt, std::string const& name, std::shared_ptr<Object> const& this_object);
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
