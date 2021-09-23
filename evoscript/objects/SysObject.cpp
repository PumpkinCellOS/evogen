#include <evoscript/objects/SysObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <iostream>

namespace evo::script
{

SysObject::SysObject()
{
    DEFINE_NATIVE_FUNCTION(SysObject, "read", &SysObject::read);
    DEFINE_NATIVE_FUNCTION(SysObject, "write", &SysObject::write);
    DEFINE_NATIVE_FUNCTION(SysObject, "dump", [](SysObject* container, Runtime& rt, std::vector<Value> const& args)->Value {
        for(auto& value: args)
            std::cout << value.dump_string() << std::endl;

        return Value::new_int(args.size());
    });
    DEFINE_NATIVE_FUNCTION(SysObject, "exit", [](SysObject* container, Runtime& rt, std::vector<Value> const& args)->Value {
        auto exit_code = args.size() == 1 ? args[0].to_int(rt) : 0;
        if(rt.has_exception())
            return {};

        // TODO: Use real stack unwinding here when exceptions are objects
        rt.~Runtime();
        ::exit(exit_code);
        assert(false);
    });
    DEFINE_NATIVE_FUNCTION(SysObject, "backtrace", &SysObject::backtrace);
    DEFINE_NATIVE_FUNCTION(SysObject, "call_system", &SysObject::call_system);
}

Value SysObject::read(Runtime& rt, std::vector<Value> const& args)
{
    std::string out;
    if(!std::getline(std::cin, out))
    {
        rt.throw_exception("Failed to read from stream");
        return {};
    }
    return StringObject::create_value(out);
}

Value SysObject::write(Runtime& rt, std::vector<Value> const& args)
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

Value SysObject::backtrace(Runtime& rt, std::vector<Value> const&)
{
    rt.print_backtrace();
    return Value::undefined();
}

Value SysObject::call_system(Runtime& rt, std::vector<Value> const& args)
{
    if(args.size() < 1)
    {
        rt.throw_exception("Missing argument: command");
        return {};
    }
    auto command = args[0];
    int code = system(command.to_string().c_str());
    return Value::new_int(code);
}

}
