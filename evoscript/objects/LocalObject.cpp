#include <evoscript/objects/LocalObject.h>

#include <sstream>

namespace evo::script
{

Value LocalObject::get(std::string const& member)
{
    auto it = m_values.find(member);
    if(it == m_values.end())
    {
        if(m_parent)
            return m_parent->get(member);
        return {};
    }

    return Value::new_reference(std::move(it->second));
}

std::shared_ptr<MemoryValue> LocalObject::allocate(std::string const& name)
{
    return m_values.insert(std::make_pair(name, MemoryValue::create_undefined())).first->second;
}

}
