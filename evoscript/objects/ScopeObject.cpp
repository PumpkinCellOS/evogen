#include <evoscript/objects/ScopeObject.h>

#include <iostream>
#include <sstream>

namespace evo::script
{

Value ScopeObject::get(StringId member)
{
    auto it = values().find(member);
    if(it == values().end())
    {
        if(m_parent)
            return m_parent->get(member);
        return {};
    }

    return Value::new_reference(std::move(it->second));
}

std::shared_ptr<MemoryValue> ScopeObject::allocate(StringId name)
{
    return values().insert(std::make_pair(name, MemoryValue::create_undefined())).first->second;
}

}
