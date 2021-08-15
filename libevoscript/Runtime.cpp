#include <libevoscript/Runtime.h>

#include <iostream>

namespace evo::script
{

Runtime::Runtime(std::shared_ptr<MemoryValue> global_object, std::shared_ptr<MemoryValue> global_this)
: m_global_object(global_object), m_global_this(global_this)
{
    if(!m_global_object)
    {
        std::cout << "Creating new global object as it was not specified" << std::endl;
        m_global_object = MemoryValue::create_object<MapObject>();
    }
    if(!m_global_this)
    {
        std::cout << "Creating new global this as it was not specified" << std::endl;
        m_global_this = MemoryValue::create_object<MapObject>();
    }

    auto& context = push_execution_context(m_global_this->value().to_object(*this));
    if(has_exception())
        return;

    auto _this = context.this_object();
    auto _global = global_object;
    auto _local = context.local_scope_object();

    std::cout << "this = " << _this->dump_string() << std::endl;
    std::cout << "global object = " << *_global << std::endl;
    std::cout << "local scope = " << *_local << std::endl;
}

Runtime::~Runtime()
{
    auto& context = current_execution_context();
    auto _this = context.this_object();
    auto _global = global_object();
    auto _local = context.local_scope_object();

    std::cout << "this = " << _this->dump_string() << std::endl;
    std::cout << "global object = " << *_global << std::endl;
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
    if(!this_object)
    {
        assert(m_global_this->value().is_object());
        this_object = m_global_this->value().get_object();
    }
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
