#include <libevoscript/objects/StringObject.h>

#include <libevoscript/AbstractOperations.h>
#include <libevoscript/NativeFunction.h>

namespace evo::script
{

Value StringObject::get(std::string const& member)
{
    NATIVE_FUNCTION(StringObject, "length", length);
    NATIVE_FUNCTION(StringObject, "concat", concat);
    NATIVE_FUNCTION(StringObject, "substring", substring);
    NATIVE_FUNCTION(StringObject, "append", append);
    return Value::undefined();
}

Value StringObject::to_primitive(Runtime& rt, Value::Type type) const
{
    return {};
}

Value StringObject::operator_add(Runtime& rt, Value const& rhs) const
{
    return concat(rt, *this, {rhs});
}

CompareResult StringObject::operator_compare(Runtime& rt, Value const& rhs) const
{
    auto rhs_string = rhs.to_string();
    return m_string == rhs_string ? CompareResult::Equal : (m_string < rhs_string ? CompareResult::Less : CompareResult::Greater);
}

Value StringObject::length(Runtime&, StringObject const& container, std::vector<Value> const& args)
{
    return Value::new_int(container.m_string.length());
}

Value StringObject::concat(Runtime& rt, StringObject const& container, std::vector<Value> const& args)
{
    std::string result = container.m_string;
    for(auto& arg: args)
        result += arg.to_string();

    return Value::new_object(std::make_shared<StringObject>(result));
}

Value StringObject::substring(Runtime& rt, StringObject const& container, std::vector<Value> const& args)
{
    if(args.size() < 1 || args.size() > 2)
    {
        rt.throw_exception("Invalid argument count");
        return {};
    }
    auto start = args[0].to_int(rt);
    if(rt.has_exception())
        return {};

    auto seq_length = args.size() == 2 ? args[1].to_int(rt) : -1;
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

    if(static_cast<size_t>(start) > container.m_string.size())
    {
        rt.throw_exception("start exceeds string length");
        return {};
    }
    if(seq_length != -1 && static_cast<size_t>(start + seq_length) > container.m_string.size())
    {
        rt.throw_exception("seq_length exceeds string length");
        return {};
    }

    return Value::new_object(std::make_shared<StringObject>(seq_length == -1 ? container.m_string.substr(start) : container.m_string.substr(start, seq_length)));
}

Value StringObject::append(Runtime& rt, StringObject& container, std::vector<Value> const& args)
{
    for(auto& arg: args)
        container.m_string.append(arg.to_string());

    // TODO: Return reference to this
    return Value::undefined();
}

}
