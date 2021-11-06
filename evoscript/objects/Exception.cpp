#include <evoscript/objects/Exception.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

Exception::Exception()
: Class("Exception")
{
    define_native_function<Exception>("print", [](Runtime& rt, Object& object, ArgumentList const&) {
        object.repl_print(std::cerr, false);
        return Value::undefined();
    });
    /*
    define_native_function<Exception>("message", [](Runtime& rt, Object& object, ArgumentList const&) {
        return Object::create_native<String>(&rt, object.internal_data<InternalData>().message);
    });*/
}

void Exception::print(Object const& object, std::ostream& output, bool detailed, bool dump) const
{
    auto& data = object.internal_data<InternalData>();
    if(dump)
    {
        Class::print(object, output, detailed, dump);
        return;
    }
    output << "\e[1m" << escapes::error(name()) << "\e[0m: " << data.message << std::endl;
    if(detailed)
        data.call_stack.print(output);
}

}
