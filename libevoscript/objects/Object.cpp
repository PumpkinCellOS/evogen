#include <libevoscript/objects/Object.h>

#include <libevoscript/NativeFunction.h>
#include <libevoscript/Runtime.h>

#include <sstream>

namespace evo::script
{

Value Object::call(Runtime& rt, Object&, std::vector<Value> const&)
{
    rt.throw_exception("Cannot call non-callable object");
    return {};
}

Value MapObject::get(std::string const& member)
{
    if(member == "length")
    {
        return Value::new_object(std::make_shared<NativeFunction<MapObject>>([](Runtime& rt, MapObject& container, std::vector<Value> const&)->Value {
            return Value::new_int(container.m_values.size());
        }));
    }

    auto it = m_values.find(member);
    if(it == m_values.end())
    {
        // Reference to newly-created undefined.
        // TODO: Tidy up these undefineds after exiting an expression
        return Value::new_reference(m_values.insert(std::make_pair(member, MemoryValue::create_undefined())).first->second);
    }

    return Value::new_reference(it->second);
}

std::string MapObject::dump_string() const
{
    std::ostringstream oss;
    for(auto& value: m_values)
    {
        if(value.second->value().is_object() && value.second->value().get_object().get() == this)
            oss << value.first << ": <recursive reference>, ";
        else
            oss << value.first << ": " << value.second->dump_string() << ", ";
    }
    return oss.str();
}

Value Function::get(std::string const&)
{
    return Value::undefined();
}

}
