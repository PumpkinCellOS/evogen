#include <evoscript/objects/Array.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

Array::Array()
: Class("Array")
{
    static StringId size_sid = "size";
    define_native_function<Array>(size_sid, &Array::size);
}

void Array::constructor(Runtime& rt, NativeObject<Array>& object, ArgumentList const& args) const
{
    auto& data = object.internal_data();
    if(args.is_given(0))
    {
        auto size = args.get(0).to_int(rt);
        if(rt.has_exception())
            return;
        data.values.resize(size);
    }
}

void Array::init_static_class_members(Object& class_wrapper)
{
    class_wrapper.define_native_function<ClassWrapper>("from_values", [](Runtime& rt, Object&, ArgumentList const& args) {
        return Value::new_object(Array::from_argument_list(args));
    });
}

std::shared_ptr<Object> Array::from_argument_list(ArgumentList const& vector)
{
    auto array = Object::create_native<Array>(nullptr);
    auto& values = array->internal_data().values;
    for(auto& value: vector)
        values.push_back(std::make_shared<MemoryValue>(value));
    return array;
}

std::shared_ptr<Object> Array::from_vector(std::vector<Value> const& vector)
{
    auto array = Object::create_native<Array>(nullptr);
    auto& values = array->internal_data().values;
    for(auto& value: vector)
        values.push_back(std::make_shared<MemoryValue>(value));
    return array;
}

Value Array::operator_subscript(Runtime& rt, Object& object, Value const& rhs) const
{
    auto& values = object.internal_data<InternalData>().values;
    auto index = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(index < 0 || static_cast<size_t>(index) >= values.size())
    {
        rt.throw_exception<Exception>("Index out of range");
        return {};
    }

    auto& value = values[index];
    if(!value)
        value = MemoryValue::create_undefined();

    return Value::new_reference(value, &object);
}

void Array::print(Object const& object, std::ostream& out, bool print_members, bool dump) const
{
    auto& values = object.internal_data<InternalData>().values;
    out << escapes::type("Array") << "(" << escapes::literal(std::to_string(values.size())) << ") ";
    if(!print_members)
    {
        out << "[...]";
        return;
    }
    out << "[";
    size_t object_count = std::min((size_t)10, values.size());
    for(size_t s = 0; s < object_count; s++)
    {
        auto& value = values[s];
        if(!value)
            out << escapes::constant("undefined");
        else
            values[s]->repl_print(out, false);
        if(s < object_count - 1)
            out << ", ";
    }
    if(object_count < values.size())
        out << ", ...";
    out << "]";
}

Value Array::size(Runtime&, Object& object, ArgumentList const&)
{
    return Value::new_int(object.internal_data<InternalData>().values.size());
}

}
