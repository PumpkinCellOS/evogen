#include <libevoscript/objects/SysObject.h>

#include <libevoscript/NativeFunction.h>
#include <libevoscript/Runtime.h>

#include <iostream>

namespace evo::script
{

Value SysObject::get(std::string const& member)
{
    NATIVE_FUNCTION(SysObject, "read", read);
    NATIVE_FUNCTION(SysObject, "write", write);
    return Value::undefined();
}

Value SysObject::read(Runtime& rt, SysObject&, std::vector<Value> const& args)
{
    std::string out;
    if(!std::getline(std::cin, out))
    {
        rt.throw_exception("Failed to read from stream");
        return {};
    }
    return Value::new_string(out);
}

Value SysObject::write(Runtime& rt, SysObject&, std::vector<Value> const& args)
{
    for(auto& it: args)
    {
        if(!(std::cout << it.to_string()))
        {
            rt.throw_exception("Failed to write to stream");
            return {};
        }
    }
    std::cout << std::endl;
    return Value::undefined();
}

}
