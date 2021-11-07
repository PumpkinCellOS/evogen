#include <evoscript/objects/String.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

String::String()
: Class("String")
{
    define_native_function<String>("length", [](Runtime&, Object& object, ArgumentList const&) {
        return Value::new_int(object.internal_data<InternalData>().string.size());
    });
    define_native_function<String>("concat", concat);
    define_native_function<String>("substring", substring);
}

void String::init_static_class_members(Object& class_wrapper)
{
    class_wrapper.define_native_function<ClassWrapper>("from_codepoints", [](Runtime& rt, Object&, ArgumentList const& args) {
        std::string result;
        // TODO: Load from array
        for(auto& val: args)
        {
            result += static_cast<char>(val.to_int(rt));
            if(rt.has_exception())
                return Value();
        }
        return Value::new_object(Object::create_native<String>(&rt, result));
    });
}

std::string String::to_string(Object const& object) const
{
    return object.internal_data<InternalData>().string;
}

Value String::to_primitive(Runtime& rt, Object const& object, Value::Type type) const
{
    auto string = object.internal_data<InternalData>().string;
    if(type == Value::Type::Int)
    {
        try
        {
            auto as_int = std::stoi(string);
            return Value::new_int(as_int);
        }
        catch(...)
        {
            rt.throw_exception<Exception>("Cannot convert '" + string + "' to int");
            return {};
        }
    }
    return {};
}

Value String::operator_add(Runtime& rt, Object const& object, Value const& rhs) const
{
    return concat(rt, object, ArgumentList{{rhs}});
}

CompareResult String::operator_compare(Runtime& rt, Object const& object, Value const& rhs) const
{
    auto& string = object.internal_data<InternalData>().string;
    auto rhs_string = rhs.to_string();
    return string == rhs_string ? CompareResult::Equal : (string < rhs_string ? CompareResult::Less : CompareResult::Greater);
}

Value String::operator_subscript(Runtime& rt, Object& object, Value const& rhs) const
{
    std::string string = object.internal_data<InternalData>().string;
    int index = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(index >= static_cast<Value::IntType>(string.size()) || index < 0)
    {
        std::ostringstream oss_rhs;
        rhs.repl_print(oss_rhs, false);
        rt.throw_exception<Exception>("Index " + oss_rhs.str() + " out of range");
        return {};
    }
    return Value::new_object(Object::create_native<String>(&rt, std::string{string[index]}));
}

Value String::concat(Runtime& rt, Object const& object, ArgumentList const& args)
{
    std::string string = object.internal_data<InternalData>().string;
    for(auto& arg: args)
        string += arg.to_string();

    return Value::new_object(Object::create_native<String>(&rt, string));
}

Value String::substring(Runtime& rt, Object const& object, ArgumentList const& args)
{
    if(!args.is_given(0))
    {
        rt.throw_exception<Exception>("You need to give at least 1 argument");
        return {};
    }
    auto start = args.get(0).to_int(rt);
    if(rt.has_exception())
        return {};

    auto seq_length = args.get_or(1, Value::new_int(-1)).to_int(rt);
    if(rt.has_exception())
        return {};
    
    if(start < 0)
    {
        rt.throw_exception<Exception>("start cannot be negative");
        return {};
    }
    if(seq_length < -1)
    {
        rt.throw_exception<Exception>("seq_length cannot be negative");
        return {};
    }

    auto const& string = object.internal_data<InternalData>().string;
    if(static_cast<size_t>(start) > string.size())
    {
        rt.throw_exception<Exception>("start exceeds string length");
        return {};
    }
    if(seq_length != -1 && static_cast<size_t>(start + seq_length) > string.size())
    {
        rt.throw_exception<Exception>("seq_length exceeds string length");
        return {};
    }

    return Value::new_object(Object::create_native<String>(&rt, seq_length == -1 ? string.substr(start) : string.substr(start, seq_length)));
}

void String::print(Object const& object, std::ostream& output, bool, bool) const
{
    // TODO: Escape this string
    output << escapes::literal("\"" + object.internal_data<InternalData>().string + "\"");
}

}
