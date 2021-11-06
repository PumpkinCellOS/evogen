#include <evoscript/objects/System.h>

#include <evoscript/Runtime.h>
#include <evoscript/objects/NativeFunction.h>
#include <evoscript/objects/String.h>

#include <iostream>

namespace evo::script
{

System::System()
: Class("System")
{
    define_native_function<System>("read", read);
    define_native_function<System>("write", write);
    define_native_function<System>("writeln", writeln);
    define_native_function<System>("dump", [](Runtime& rt, Object&, ArgumentList const& args)->Value {
        for(auto& value: args)
            std::cout << value.dump_string() << std::endl;

        return Value::new_int(args.size());
    });
    define_native_function<System>("exit", [](Runtime& rt, Object&, ArgumentList const& args)->Value {
        auto exit_code = args.get_or(0, Value::new_int(-1)).to_int(rt);
        if(rt.has_exception())
            return {};

        // TODO: Use real stack unwinding here when exceptions are objects
        rt.~Runtime();
        ::exit(exit_code);
        assert(false);
    });
    define_native_function<System>("backtrace", backtrace);
    define_native_function<System>("call_system", call_system);
    define_native_function<System>("cwd", cwd);
}

Value System::read(Runtime& rt, Object&, ArgumentList const& args)
{
    std::string out;
    if(!std::getline(std::cin, out))
    {
        rt.throw_exception<Exception>("Failed to read from stream");
        return {};
    }
    return Value::new_object(Object::create_native<String>(&rt, out));
}

Value System::write(Runtime& rt, Object&, ArgumentList const& args)
{
    for(auto& it: args)
    {
        if(!(std::cout << it.to_string()))
        {
            rt.throw_exception<Exception>("Failed to write to stream");
            return {};
        }
    }
    return Value::undefined();
}

Value System::writeln(Runtime& rt, Object& object, ArgumentList const& args)
{
    write(rt, object, args);
    std::cout << std::endl;
    return Value::undefined();
}

Value System::backtrace(Runtime& rt, Object&, ArgumentList const&)
{
    rt.print_backtrace();
    return Value::undefined();
}

Value System::call_system(Runtime& rt, Object&, ArgumentList const& args)
{
    if(!args.is_given(0))
    {
        rt.throw_exception<Exception>("Missing argument: command");
        return {};
    }
    int code = system(args.get(0).to_string().c_str());
    return Value::new_int(code);
}

Value System::cwd(Runtime& rt, Object&, ArgumentList const&)
{
    #ifdef __unix__
        char* buffer = getcwd(nullptr, 0);
        std::string result(buffer);
        free(buffer);
        return Value::new_object(Object::create_native<String>(&rt, result));
    #else
        return Value::undefined();
    #endif
}

}
