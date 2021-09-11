#include <evoscript/objects/Exception.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/StringObject.h>
#include <iostream>

namespace evo::script
{

Exception::Exception(Runtime& rt, std::string const& message)
: m_call_stack(rt.call_stack()), m_message(message)
{
    DEFINE_NATIVE_FUNCTION(Exception, "print", print_);
    DEFINE_NATIVE_OBJECT(object, "message", std::make_shared<StringObject>(m_message));
}

void Exception::print()
{
    std::cerr << "\e[1m" << type_name() << "\e[0m: " << m_message << std::endl;
    m_call_stack.print();
}

Value Exception::print_(Runtime& rt, Exception& container, std::vector<Value> const&)
{
    container.print();
    return Value::undefined();
}

}
