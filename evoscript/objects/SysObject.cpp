#include <evoscript/objects/SysObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <iostream>

namespace evo::script
{

SysObject::SysObject()
{
    DEFINE_NATIVE_FUNCTION(SysObject, "read", read);
    DEFINE_NATIVE_FUNCTION(SysObject, "write", write);
    DEFINE_NATIVE_FUNCTION(SysObject, "dump", [](Runtime& rt, SysObject& container, std::vector<Value> const& args)->Value {
        for(auto& value: args)
            std::cout << value.dump_string() << std::endl;

        return Value::new_int(args.size());
    });
    DEFINE_NATIVE_FUNCTION(SysObject, "exit", [](Runtime& rt, SysObject& container, std::vector<Value> const& args)->Value {
        auto exit_code = args.size() == 1 ? args[0].to_int(rt) : 0;
        if(rt.has_exception())
            return {};

        // TODO: Use real stack unwinding here when exceptions are objects
        rt.~Runtime();
        ::exit(exit_code);
        assert(false);
    });
    DEFINE_NATIVE_FUNCTION(SysObject, "backtrace", backtrace);
}

Value SysObject::read(Runtime& rt, SysObject&, std::vector<Value> const& args)
{
    std::string out;
    if(!std::getline(std::cin, out))
    {
        rt.throw_exception("Failed to read from stream");
        return {};
    }
    return Value::new_object(std::make_shared<StringObject>(out));
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

Value SysObject::backtrace(Runtime& rt, SysObject&, std::vector<Value> const&)
{
    rt.print_backtrace();
    return Value::undefined();
}

}
