#include <libevoscript/objects/Exception.h>

#include <libevoscript/NativeFunction.h>
#include <libevoscript/objects/StringObject.h>
#include <iostream>

namespace evo::script
{

Value Exception::get(std::string const& member)
{
    NATIVE_FUNCTION(Exception, "print", print_);
    NATIVE_OBJECT(object, "message", std::make_shared<StringObject>(m_message));
    return Value::undefined();
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
