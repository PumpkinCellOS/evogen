#include <evoscript/objects/Object.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <sstream>

namespace evo::script
{

Value Object::get(std::string const& member)
{
    auto it = m_values.find(member);
    if(it == m_values.end())
    {
        // Reference to newly-created undefined.
        // TODO: Tidy up these undefineds after exiting an expression
        return Value::new_reference(m_values.insert(std::make_pair(member, MemoryValue::create_undefined())).first->second);
    }

    return Value::new_reference(it->second);
}

std::string Object::repl_string() const
{
    std::ostringstream oss;
    oss << type_name() << " { ";
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

CompareResult Object::operator_compare(Runtime& rt, Value const& rhs) const
{
    rt.throw_exception("Cannot call operator<> on object lhs=" + repl_string() + " with rhs=" + rhs.repl_string());
    return {};
}

Function::Function(std::string const& name)
: m_name(name)
{
    DEFINE_READ_ONLY_NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name));
}

}
