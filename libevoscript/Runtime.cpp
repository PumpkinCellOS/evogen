#include "libevoscript/CallStack.h"
#include <libevoscript/Runtime.h>

#include <libevoscript/objects/Exception.h>
#include <libevoscript/objects/MapObject.h>
#include <libevoscript/objects/Object.h>

#include <iostream>

namespace evo::script
{

Runtime::Runtime(std::shared_ptr<GlobalObject> global_object, std::shared_ptr<MemoryValue> global_this)
: m_global_object(global_object), m_global_this(global_this)
{
    if(!m_global_object)
    {
        std::cerr << "Creating new global object as it was not specified" << std::endl;
        m_global_object = std::make_shared<GlobalObject>();
    }
    if(!m_global_this)
    {
        std::cerr << "Creating new global this as it was not specified" << std::endl;
        m_global_this = MemoryValue::create_object<MapObject>();
    }

    auto& context = m_call_stack.push_execution_context("<global scope>", m_global_this->value().to_object(*this));
    if(has_exception())
        return;

    auto _this = context.this_object();
    auto _global = global_object;
    auto _local = context.local_scope_object();

    std::cerr << "this = " << _this->dump_string() << std::endl;
    std::cerr << "global object = " << _global->dump_string() << std::endl;
    std::cerr << "local scope = " << _local->dump_string() << std::endl;
}

Runtime::~Runtime()
{
    auto& context = m_call_stack.current_execution_context();
    auto _this = context.this_object();
    auto _global = global_object();
    auto _local = context.local_scope_object();

    std::cerr << "this = " << _this->dump_string() << std::endl;
    std::cerr << "global object = " << _global->dump_string() << std::endl;
    std::cerr << "local scope = " << _local->dump_string() << std::endl;

    m_call_stack.pop_execution_context();
}

void Runtime::throw_exception(std::string const& message)
{
    m_exception = std::make_shared<Exception>(*this, message);
}

void Runtime::print_backtrace() const
{
    std::cerr << "Backtrace:" << std::endl;
    m_call_stack.print();
}

ExecutionContext& Runtime::push_execution_context(std::string const& name, std::shared_ptr<Object> this_object)
{
    if(!this_object)
    {
        assert(m_global_this->value().is_object());
        this_object = m_global_this->value().get_object();
    }
    return m_call_stack.push_execution_context(name, this_object);
}

ExecutionContext& Runtime::push_scope()
{
    return m_call_stack.push_scope(this_object());
}

ExecutionContext& Runtime::current_execution_context()
{
    return m_call_stack.current_execution_context();
}

void Runtime::pop_execution_context()
{
    m_call_stack.pop_execution_context();
}

}
