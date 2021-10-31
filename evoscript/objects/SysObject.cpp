#include "evoscript/objects/Object.h"
#include <evoscript/objects/SysObject.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/StringObject.h>

#include <iostream>

namespace evo::script
{

SysObject::SysObject()
{
    define_native_function<SysObject>("read", &SysObject::read);
    define_native_function<SysObject>("write", &SysObject::write);
    define_native_function<SysObject>("writeln", &SysObject::writeln);
    define_native_function<SysObject>("dump", [](SysObject* container, Runtime& rt, ArgumentList const& args)->Value {
        for(auto& value: args)
            std::cout << value.dump_string() << std::endl;

        return Value::new_int(args.size());
    });
    define_native_function<SysObject>("exit", [](SysObject* container, Runtime& rt, ArgumentList const& args)->Value {
        auto exit_code = args.get_or(0, Value::new_int(-1)).to_int(rt);
        if(rt.has_exception())
            return {};

        // TODO: Use real stack unwinding here when exceptions are objects
        rt.~Runtime();
        ::exit(exit_code);
        assert(false);
    });
    define_native_function<SysObject>("backtrace", &SysObject::backtrace);
    define_native_function<SysObject>("call_system", &SysObject::call_system);
    define_native_function<SysObject>("cwd", &SysObject::cwd);
}

Value SysObject::read(Runtime& rt, ArgumentList const& args)
{
    std::string out;
    if(!std::getline(std::cin, out))
    {
        rt.throw_exception("Failed to read from stream");
        return {};
    }
    return new_object_value<StringObject>(out);
}

Value SysObject::write(Runtime& rt, ArgumentList const& args)
{
    for(auto& it: args)
    {
        if(!(std::cout << it.to_string()))
        {
            rt.throw_exception("Failed to write to stream");
            return {};
        }
    }
    return Value::undefined();
}

Value SysObject::writeln(Runtime& rt, ArgumentList const& args)
{
    write(rt, args);
    std::cout << std::endl;
    return Value::undefined();
}

Value SysObject::backtrace(Runtime& rt, ArgumentList const&)
{
    rt.print_backtrace();
    return Value::undefined();
}

Value SysObject::call_system(Runtime& rt, ArgumentList const& args)
{
    if(!args.is_given(0))
    {
        rt.throw_exception("Missing argument: command");
        return {};
    }
    int code = system(args.get(0).to_string().c_str());
    return Value::new_int(code);
}

Value SysObject::cwd(Runtime&, ArgumentList const&)
{
    #ifdef __unix__
        char* buffer = getcwd(nullptr, 0);
        std::string result(buffer);
        free(buffer);
        return new_object_value<StringObject>(result);
    #else
        return Value::undefined();
    #endif
}

}
