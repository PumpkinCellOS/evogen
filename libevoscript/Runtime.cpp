#include <libevoscript/Runtime.h>

#include <iostream>

namespace evo::script
{

Runtime::Runtime(std::shared_ptr<MemoryValue> global_this)
: m_global_this(global_this)
{
    if(!m_global_this)
    {
        std::cout << "Creating new global this as it was not specified" << std::endl;
        m_global_this = MemoryValue::create_object<MapObject>();
    }

    auto& context = push_execution_context(m_global_this->value().to_object(*this));
    if(has_exception())
        return;

    // some test
    // TODO: This should be given by user with global_this argument
    auto _this = context.this_object();
    auto _local = context.local_scope_object();
    _this->get("testInt").assign(Value::new_int(1234));
    _this->get("testObject").assign(Value::new_object(std::make_shared<MapObject>()));
    _local = MemoryValue::create_object<MapObject>();
    _local->value().get_object()->get("testNull").assign(Value::null());
    _local->value().get_object()->get("testReference").assign_direct(Value::new_reference(MemoryValue::create_existing_object(_this)));

    std::cout << "this = " << _this->dump_string() << std::endl;
    std::cout << "local scope = " << *_local << std::endl;
}

Runtime::~Runtime()
{
    auto& context = current_execution_context();
    auto _this = context.this_object();
    auto _local = context.local_scope_object();

    std::cout << "this = " << _this->dump_string() << std::endl;
    std::cout << "local scope = " << *_local << std::endl;

    pop_execution_context();
}

void Runtime::throw_exception(std::string const& message)
{
    m_exception_message = message;
    std::cerr << "evoscript: VM EXCEPTION: " << message << std::endl;
}

ExecutionContext& Runtime::push_execution_context(std::shared_ptr<Object> this_object)
{
    return m_execution_context_stack.emplace(this_object);
}

ExecutionContext& Runtime::current_execution_context()
{
    return m_execution_context_stack.top();
}

void Runtime::pop_execution_context()
{
    m_execution_context_stack.pop();
}

}
