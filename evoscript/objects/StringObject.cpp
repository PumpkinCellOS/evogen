#include <evoscript/objects/StringObject.h>

#include <evoscript/AbstractOperations.h>
#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/objects/Class.h>

namespace evo::script
{

StringObject::StringObject(std::string const& str)
: m_string(str)
{
    static StringId length_sid = "length";
    define_native_function<StringObject>(length_sid, &StringObject::length);
    static StringId concat_sid = "concat";
    define_native_function<StringObject>(concat_sid, &StringObject::concat);
    static StringId substring_sid = "substring";
    define_native_function<StringObject>(substring_sid, &StringObject::substring);
    static StringId append_sid = "append";
    define_native_function<StringObject>(append_sid, &StringObject::append);
}

void StringObject::init_class(Class& class_)
{
    static StringId from_codepoints_sid = "from_codepoints";
    class_.define_native_function<Class>(from_codepoints_sid, [](Class*, Runtime& rt, ArgumentList const& args) {
        std::string result;
        // TODO: Load from array
        for(auto& val: args)
        {
            result += static_cast<char>(val.to_int(rt));
            if(rt.has_exception())
                return Value();
        }
        return new_object_value<StringObject>(result);
    });
}

void StringObject::repl_print(std::ostream& output, bool) const
{
    // TODO: Escape this string
    using namespace escapes;
    output << literal("\"" + m_string + "\"");
}

Value StringObject::to_primitive(Runtime& rt, Value::Type type) const
{
    if(type == Value::Type::Int)
    {
        try
        {
            auto as_int = std::stoi(m_string);
            return Value::new_int(as_int);
        }
        catch(...)
        {
            rt.throw_exception("Cannot convert '" + m_string + "' to int");
            return {};
        }
    }
    return {};
}

Value StringObject::operator_add(Runtime& rt, Value const& rhs) const
{
    return concat(rt, ArgumentList{{rhs}});
}

CompareResult StringObject::operator_compare(Runtime& rt, Value const& rhs) const
{
    auto rhs_string = rhs.to_string();
    return m_string == rhs_string ? CompareResult::Equal : (m_string < rhs_string ? CompareResult::Less : CompareResult::Greater);
}

Value StringObject::operator_subscript(Runtime& rt, Value const& rhs)
{
    int index = rhs.to_int(rt);
    if(rt.has_exception())
        return {};

    if(index >= static_cast<Value::IntType>(m_string.size()) || index < 0)
    {
        std::ostringstream oss_rhs;
        rhs.repl_print(oss_rhs, false);
        rt.throw_exception("Index " + oss_rhs.str() + " out of range");
        return {};
    }
    return new_object_value<StringObject>(std::string{m_string[index]});
}

Value StringObject::length(Runtime&, ArgumentList const& args) const
{
    return Value::new_int(m_string.length());
}

Value StringObject::concat(Runtime& rt, ArgumentList const& args) const
{
    std::string result = m_string;
    for(auto& arg: args)
        result += arg.to_string();

    return new_object_value<StringObject>(result);
}

Value StringObject::substring(Runtime& rt, ArgumentList const& args) const
{
    if(!args.is_given(0))
    {
        rt.throw_exception("You need to give at least 1 argument");
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
        rt.throw_exception("start cannot be negative");
        return {};
    }
    if(seq_length < -1)
    {
        rt.throw_exception("seq_length cannot be negative");
        return {};
    }

    if(static_cast<size_t>(start) > m_string.size())
    {
        rt.throw_exception("start exceeds string length");
        return {};
    }
    if(seq_length != -1 && static_cast<size_t>(start + seq_length) > m_string.size())
    {
        rt.throw_exception("seq_length exceeds string length");
        return {};
    }

    return new_object_value<StringObject>(seq_length == -1 ? m_string.substr(start) : m_string.substr(start, seq_length));
}

Value StringObject::append(Runtime& rt, ArgumentList const& args)
{
    for(auto& arg: args)
        m_string.append(arg.to_string());

    // TODO: Return reference to this
    return Value::undefined();
}

}
