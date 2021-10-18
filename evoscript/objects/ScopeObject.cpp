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
    auto memory_value = MemoryValue::create_undefined();
    memory_value->set_name(name);
    return values().insert(std::make_pair(name, std::move(memory_value))).first->second;
}

}
