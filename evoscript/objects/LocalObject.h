#pragma once

#include <evoscript/objects/Object.h>
#include <map>

namespace evo::script
{

class LocalObject : public Object
{
public:
    LocalObject(std::shared_ptr<LocalObject> parent)
    : m_parent(parent) {}

    EVO_OBJECT("LocalObject")

    virtual Value get(std::string const& member) override;
    std::shared_ptr<MemoryValue> allocate(std::string const& name);
    std::shared_ptr<LocalObject> parent() const { return m_parent; }

private:
    std::unordered_map<std::string, std::shared_ptr<MemoryValue>> m_values;
    std::shared_ptr<LocalObject> m_parent;
};

}
