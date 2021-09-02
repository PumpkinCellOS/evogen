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

Value Object::operator_add(Runtime& rt, Value const& rhs) const
{
    rt.throw_exception("Cannot call operator+ on object lhs=" + repl_string() + " with rhs=" + rhs.repl_string());
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

Value Function::get(std::string const&)
{
    return Value::undefined();
}

}
