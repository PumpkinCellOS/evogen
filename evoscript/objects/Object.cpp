#include <evoscript/objects/Object.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <sstream>

namespace evo::script
{

Value Object::get(std::string const& member)
{
    // function type() : string
    NATIVE_FUNCTION(Object, "type", [](Runtime&, Object& container, std::vector<Value> const&) {
        return StringObject::create_value(container.type_name());
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

void Object::repl_print(std::ostream& output, bool print_members) const
{
    using namespace escapes;
    output << type(type_name()) << " {";
    if(print_members)
    {
        if(!m_values.empty())
            output << std::endl;
        size_t counter = 0;
        for(auto& value: m_values)
        {
            if(value.second->value().is_object() && value.second->value().get_object().get() == this)
                output << "    " << literal(value.first) << ": " << constant("<recursive reference>");
            else
            {
                // TODO: Use dump_string if called from dump_string
                output << "    " << literal(value.first) << ": ";
                value.second->repl_print(output, false);
            }

            if(counter < m_values.size() - 1)
                output << std::endl;
            counter++;
        }
        if(!m_values.empty())
            output << std::endl;
    }
    output << "}";
}

Value Object::call(Runtime& rt, Object&, std::vector<Value> const&)
{
    rt.throw_exception("Cannot call non-callable object");
    return {};
}

Value Object::operator_add(Runtime& rt, Value const& rhs) const
{
    std::ostringstream oss_lhs;
    repl_print(oss_lhs, true);
    std::ostringstream oss_rhs;
    repl_print(oss_rhs, true);
    rt.throw_exception("Cannot call operator+ on object lhs=" + oss_lhs.str() + " with rhs=" + oss_rhs.str());
    return {};
}

CompareResult Object::operator_compare(Runtime& rt, Value const& rhs) const
{
    std::ostringstream oss_lhs;
    repl_print(oss_lhs, true);
    std::ostringstream oss_rhs;
    repl_print(oss_rhs, true);
    rt.throw_exception("Cannot call operator<> on object lhs=" + oss_lhs.str() + " with rhs=" + oss_rhs.str());
    return {};
}

Function::Function(std::string const& name)
: m_name(name)
{
}

void Function::repl_print(std::ostream& output, bool print_members) const
{
    using namespace escapes;
    output << keyword("function") << " " << escapes::name(m_name) << "()";
}

Value Function::get(std::string const& member)
{
    NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name));
    return Object::get(member);
}

}
