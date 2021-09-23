#include <evoscript/objects/Object.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <sstream>

namespace evo::script
{

Value Object::get(StringId member)
{
    // function type() : string
    static StringId type_sid = "type";
    NATIVE_FUNCTION(Object, type_sid, [](Object* container, Runtime&, std::vector<Value> const&) {
        return new_object_value<StringObject>(container->type_name());
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
    print_impl(output, print_members, false);
}

void Object::print_impl(std::ostream& output, bool print_members, bool dump) const
{
    using namespace escapes;
    output << type(type_name()) << " ";
    if(print_members)
        print_members_impl(output, dump);
    else
        output << "{...}";
}

void Object::print_members_impl(std::ostream& output, bool dump) const
{
    using namespace escapes;
    output << "{";
    if(!m_values.empty())
        output << std::endl;
    size_t counter = 0;
    for(auto& value: m_values)
    {
        if(value.second->value().is_object() && value.second->value().get_object().get() == this)
            output << "  " << literal(value.first.string()) << ": " << constant("<recursive reference>");
        else
        {
            output << "  " << literal(value.first.string()) << ": ";
            if(dump)
                output << value.second->dump_string();
            else
                value.second->repl_print(output, false);
        }

        if(counter < m_values.size() - 1)
            output << std::endl;
        counter++;
    }
    if(!m_values.empty())
        output << std::endl;
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
    rhs.repl_print(oss_rhs, true);
    rt.throw_exception("Cannot call operator+ on object lhs = " + oss_lhs.str() + " with rhs = " + oss_rhs.str());
    return {};
}

CompareResult Object::operator_compare(Runtime& rt, Value const& rhs) const
{
    std::ostringstream oss_lhs;
    repl_print(oss_lhs, true);
    std::ostringstream oss_rhs;
    rhs.repl_print(oss_rhs, true);
    rt.throw_exception("Cannot call operator<> on object lhs = " + oss_lhs.str() + " with rhs = " + oss_rhs.str());
    return {};
}

Value Object::operator_subscript(Runtime& rt, Value const& rhs)
{
    return get(rhs.to_string());
}

Function::Function(StringId name)
: m_name(name)
{
}

void Function::repl_print(std::ostream& output, bool print_members) const
{
    output << escapes::keyword("function") << (m_name.empty() ? "" : " ") << escapes::name(m_name.string()) << "()";
}

Value Function::get(StringId member)
{
    NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name.string()));
    return Object::get(member);
}

}
