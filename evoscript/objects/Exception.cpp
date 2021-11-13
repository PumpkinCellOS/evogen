#include <evoscript/objects/Exception.h>
#include <evoscript/objects/String.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/objects/NativeFunction.h>

namespace evo::script
{

Exception::InternalData::InternalData(CallStack const& call_stack_, std::string const& message_)
: message(message_)
{
    call_stack_.walk_stacktrace([this](ExecutionContext const& ctx) {
        stack_frames.push_back(ctx.name());
    });
}

Exception::Exception()
: Class("Exception")
{
    define_native_function<Exception>("print", [](Runtime& rt, Object& object, ArgumentList const&) {
        object.repl_print(std::cerr, false);
        return Value::undefined();
    });
    define_native_function<Exception>("message", [](Runtime& rt, Object& object, ArgumentList const&) {
        return Value::new_object(Object::create_native<String>(&rt, object.internal_data<InternalData>().message));
    });
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
    {
        for(auto const& frame: data.stack_frames)
            std::cout << "    at " << escapes::name(frame) << std::endl;
    }
}

}
