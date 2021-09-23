#include <evoscript/objects/Array.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>

namespace evo::script
{

Array::Array(Runtime& rt, std::vector<Value> const& args)
{
    static StringId size_sid = "size";
    DEFINE_NATIVE_FUNCTION(Array, size_sid, &Array::size);
    auto size = (args.size() >= 1 ? args[0] : Value::undefined());
    auto size_int = size.to_int(rt);
    if(rt.has_exception())
        return;
    m_values.resize(size_int);

    // TODO: Do it lazily
    for(auto& value: m_values)
        value = MemoryValue::create_undefined();
}

Value Array::operator_subscript(Runtime& rt, Value const& rhs)
{
    auto index = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(index < 0 || static_cast<size_t>(index) >= m_values.size())
    {
        rt.throw_exception("Index out of range");
        return {};
    }
    return Value::new_reference(m_values[index]);
}

void Array::repl_print(std::ostream& out, bool print_members) const
{
    if(!print_members)
    {
        out << escapes::type("Array");
        return;
    }
    out << escapes::type("Array") << "(" << escapes::literal(std::to_string(m_values.size())) << ") [";
    size_t object_count = std::min((size_t)4, m_values.size());
    for(size_t s = 0; s < object_count; s++)
    {
        m_values[s]->repl_print(out, false);
        if(s < object_count - 1)
            out << ", ";
    }
    if(object_count < m_values.size())
        out << ", ...";
    out << "]";
}

Value Array::size(Runtime&, std::vector<Value> const&) const
{
    return Value::new_int(m_values.size());
}

}
