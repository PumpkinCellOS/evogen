#include <libevoscript/Runtime.h>

#include <iostream>

namespace evo::script
{

Runtime::Runtime()
{
    m_this = MemoryValue::create_object<MapObject>();
    m_this->value().get_object()->get("testInt").assign(Value::new_int(1234));
    m_local_scope = MemoryValue::create_object<MapObject>();
    m_local_scope->value().get_object()->get("testNull").assign(Value::null());
    m_local_scope->value().get_object()->get("testReference").assign_direct(Value::new_reference(m_this));
    std::cout << "this = " << *m_this << std::endl;
    std::cout << "local scope = " << *m_local_scope << std::endl;
}

Runtime::~Runtime()
{
    std::cout << "this = " << *m_this << std::endl;
    std::cout << "local scope = " << *m_local_scope << std::endl;
}

void Runtime::throw_exception(std::string const& message)
{
    m_exception_message = message;
    std::cerr << "evoscript: VM EXCEPTION: " << message << std::endl;
}

}
