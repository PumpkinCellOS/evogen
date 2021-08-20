#include "libevoscript/AbstractOperations.h"
#include <libevoscript/objects/StringObject.h>

#include <libevoscript/NativeFunction.h>

namespace evo::script
{

Value StringObject::get(std::string const& member)
{
    NATIVE_FUNCTION(StringObject, "string", string);
    NATIVE_FUNCTION(StringObject, "length", length);
    NATIVE_FUNCTION(StringObject, "concat", concat);
    NATIVE_FUNCTION(StringObject, "substring", substring);
    return Value::undefined();
}

Value StringObject::string(Runtime&, StringObject& container, std::vector<Value> const& args)
{
    return Value::new_string(container.m_string);
}

Value StringObject::length(Runtime&, StringObject& container, std::vector<Value> const& args)
{
    return Value::new_int(container.m_string.length());
}

Value StringObject::concat(Runtime& rt, StringObject& container, std::vector<Value> const& args)
{
    Value result = Value::new_string(container.m_string);
    for(auto& arg: args)
    {
        result = abstract::add(rt, result, arg);
    }
    return result;
}

Value StringObject::substring(Runtime& rt, StringObject& container, std::vector<Value> const& args)
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

    return Value::new_string(seq_length == -1 ? container.m_string.substr(start) : container.m_string.substr(start, seq_length));
}

}
