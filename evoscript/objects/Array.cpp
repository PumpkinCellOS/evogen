#include <evoscript/objects/Array.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Class.h>

namespace evo::script
{

Array::Array()
{
    static StringId size_sid = "size";
    define_native_function<Array>(size_sid, &Array::size);
}

Array::Array(Runtime& rt, ArgumentList const& args)
: Array()
{
    auto size = args.get(0);
    auto size_int = size.to_int(rt);
    if(rt.has_exception())
        return;
    m_values.resize(size_int);

    // TODO: Do it lazily
    for(auto& value: m_values)
        value = MemoryValue::create_undefined();
}

void Array::init_class(Class& class_)
{
    static StringId from_values_sid = "from_values";
    class_.define_native_function<Class>(from_values_sid, [](Class*, Runtime& rt, ArgumentList const& args) {
        return Value::new_object(Array::from_argument_list(args));
    });
}

std::shared_ptr<Array> Array::from_argument_list(ArgumentList const& vector)
{
    std::shared_ptr<Array> array = std::make_shared<Array>();
    for(auto& value: vector)
        array->m_values.push_back(std::make_shared<MemoryValue>(value));
    return array;
}

std::shared_ptr<Array> Array::from_vector(std::vector<Value> const& vector)
{
    std::shared_ptr<Array> array = std::make_shared<Array>();
    for(auto& value: vector)
        array->m_values.push_back(std::make_shared<MemoryValue>(value));
    return array;
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
    out << escapes::type("Array") << "(" << escapes::literal(std::to_string(m_values.size())) << ") ";
    if(!print_members)
    {
        out << "[...]";
        return;
    }
    out << "[";
    size_t object_count = std::min((size_t)10, m_values.size());
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

Value Array::size(Runtime&, ArgumentList const&) const
{
    return Value::new_int(m_values.size());
}

}
