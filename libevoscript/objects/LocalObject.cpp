#include <libevoscript/objects/LocalObject.h>

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

    return Value::new_reference(it->second);
}

std::shared_ptr<MemoryValue> LocalObject::allocate(std::string const& name)
{
    return m_values.insert(std::make_pair(name, MemoryValue::create_undefined())).first->second;
}

std::string LocalObject::repl_string() const
{
    std::ostringstream oss;
    oss << "{ ";
    size_t counter = 0;
    for(auto& value: m_values)
    {
        if(value.second->value().is_object() && value.second->value().get_object().get() == this)
            oss << value.first << ": <recursive reference>";
        else
            // TODO: Use dump_string if called from dump_string
            oss << value.first << ": " << value.second->repl_string();

        if(counter < m_values.size() - 1)
            oss << ", ";
        counter++;
    }
    oss << " }";
    return oss.str();
}

}
