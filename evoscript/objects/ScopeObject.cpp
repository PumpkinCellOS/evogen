#include <evoscript/objects/ScopeObject.h>

#include <iostream>
#include <sstream>

namespace evo::script
{

Value ScopeObject::get(StringId member)
{
    auto [scope, reference] = resolve_identifier(member);
    if(!reference)
        return Value::undefined();
    return reference->value();
}

ScopeObject::IdentifierRecord ScopeObject::resolve_identifier(StringId name)
{
    auto it = values().find(name);
    if(it == values().end())
    {
        if(m_parent)
            return m_parent->resolve_identifier(name);
        return {shared_from_this(), nullptr};
    }
    return {shared_from_this(), it->second};
}

std::shared_ptr<MemoryValue> ScopeObject::allocate(StringId name)
{
    auto memory_value = MemoryValue::create_undefined();
    memory_value->set_name(name);
    return values().insert(std::make_pair(name, std::move(memory_value))).first->second;
}

}
