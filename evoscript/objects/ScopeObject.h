#pragma once

#include <evoscript/objects/Object.h>
#include <map>

namespace evo::script
{

class ScopeObject : public Object, public std::enable_shared_from_this<ScopeObject>
{
public:
    ScopeObject(std::shared_ptr<ScopeObject> parent)
    : m_parent(parent) {}

    EVO_OBJECT("ScopeObject")

    virtual Value get(StringId member) override;
    std::shared_ptr<MemoryValue> allocate(StringId name);

    struct IdentifierRecord
    {
        std::shared_ptr<ScopeObject> scope;
        std::shared_ptr<MemoryValue> reference;
    };

    IdentifierRecord resolve_identifier(StringId name);
    std::shared_ptr<ScopeObject> parent() const { return m_parent; }

private:
    std::shared_ptr<ScopeObject> m_parent;
};

}
