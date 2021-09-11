#include <evoscript/objects/MapObject.h>
#include <evoscript/NativeFunction.h>

#include <sstream>

namespace evo::script
{

Value MapObject::get(std::string const& member)
{
    NATIVE_FUNCTION(MapObject, "length", [](Runtime& rt, MapObject& container, std::vector<Value> const&)->Value {
        return Value::new_int(container.m_values.size());
    });

    auto it = m_values.find(member);
    if(it == m_values.end())
    {
        // Reference to newly-created undefined.
        // TODO: Tidy up these undefineds after exiting an expression
        return Value::new_reference(m_values.insert(std::make_pair(member, MemoryValue::create_undefined())).first->second);
    }

    return Value::new_reference(it->second);
}

std::string MapObject::repl_string() const
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
