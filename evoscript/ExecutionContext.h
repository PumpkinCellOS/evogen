#pragma once

#include <evoscript/GlobalObject.h>
#include <evoscript/ScopeObject.h>
#include <evoscript/Value.h>

namespace evo::script
{

class ExecutionContext
{
public:
    ExecutionContext(std::string const& name, std::shared_ptr<Object> const& this_object, std::shared_ptr<GlobalObject> const& global_object, ScopeObject& parent_scope);

    template<class T = Object>
    std::shared_ptr<T> this_object() const { return std::static_pointer_cast<T>(m_this); }

    ScopeObject const& scope_object() const { return m_local_scope; }
    ScopeObject& scope_object() { return m_local_scope; }
    std::string name() const { return m_name; }
    ScopeObject const& parent_scope() const { return *m_local_scope.parent(); }
    ScopeObject& parent_scope() { return *m_local_scope.parent(); }
    std::shared_ptr<GlobalObject> global_object() const { return m_global_object; }

private:
    std::shared_ptr<Object> m_this;
    ScopeObject m_local_scope;
    std::shared_ptr<GlobalObject> m_global_object;
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
