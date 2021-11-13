#pragma once

#include <evoscript/Object.h>
#include <evoscript/StringId.h>
#include <evoscript/Value.h>
#include <map>
#include <memory>

namespace evo::script
{

class ScopeObject : public Object, public std::enable_shared_from_this<ScopeObject>
{
public:
    explicit ScopeObject(ScopeObject& parent)
    : m_parent(&parent) {}

    virtual std::shared_ptr<MemoryValue> get(StringId member) override;
    std::shared_ptr<MemoryValue> allocate(StringId name);

    struct IdentifierRecord
    {
        ScopeObject* scope;
        std::shared_ptr<MemoryValue> reference;
    };

    IdentifierRecord resolve_identifier(StringId name);
    ScopeObject* parent() const { return m_parent; }

protected:
    // Called only by GlobalObject
    ScopeObject() = default;

private:
    ScopeObject* m_parent { nullptr };
};

}
