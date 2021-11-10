#include <evoscript/Value.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Exception.h>

#include <cassert>
#include <sstream>

namespace evo::script
{

Value& Value::operator=(Value const& other)
{
    if(this == &other)
        return *this;
    assign_direct(other);
    return *this;
}

Value& Value::operator=(Value&& other)
{
    if(this == &other)
        return *this;
    move_assign_direct(std::forward<Value>(other));
    return *this;
}

Value::~Value()
{
    switch(m_type)
    {
    case Type::Object:
        m_value.object.~shared_ptr<Object>();
        break;
    case Type::Reference:
        m_value.reference.~Reference();
        break;
    default:
        // Nothing to do, they are primitives
        break;
    }
}

std::string Value::type_to_string(Type type)
{
    switch(type)
    {
    case Type::Invalid:
        return "<invalid>";
    case Type::Null:
        return "null";
    case Type::Undefined:
        return "undefined";
    case Type::Int:
        return "int";
    case Type::Bool:
        return "bool";
    case Type::Object:
        return "object";
    case Type::Reference:
        return "reference";
    default:
        return "<\?\?>(" + std::to_string((int)type) + ")";
    }
}
    
Value::IntType Value::to_int(Runtime& rt) const
{
    switch(m_type)
    {
    case Type::Null:
    case Type::Undefined:
        return 0;
    case Type::Object:
    {
        auto primitive = get_object()->to_primitive(rt, Value::Type::Int);
        if(primitive.is_invalid())
        {
            std::ostringstream oss;
            get_object()->repl_print(oss, false);
            rt.throw_exception<Exception>("Cannot convert " + oss.str() + " to int");
            return 0;
        }
        return primitive.to_int(rt);
    }
    case Type::Int:
        return get_int();
    case Type::Bool:
        return get_bool() ? 1 : 0;
    case Type::Reference:
        return get_reference()->value().to_int(rt);
    default:
        assert(false);
        return {};
    }
}
    
bool Value::to_bool(Runtime& rt) const
{
    switch(m_type)
    {
    case Type::Null:
    case Type::Undefined:
        return false;
    case Type::Object:
        return true;
    case Type::Int:
        return get_int() != 0;
    case Type::Bool:
        return get_bool();
    case Type::Reference:
        return get_reference()->value().to_bool(rt);
    default:
        assert(false);
        return {};
    }
}

std::shared_ptr<Object> Value::to_object(Runtime& rt) const
{
    switch(m_type)
    {
    case Type::Null:
    case Type::Undefined:
    case Type::Int:
    case Type::Bool:
        rt.throw_exception<Exception>("Cannot convert " + type_to_string(m_type) + " to object");
        return nullptr;
    case Type::Object:
        return get_object();
    case Type::Reference:
        return get_reference()->value().to_object(rt);
    default:
        assert(false);
        return {};
    }
}

std::string Value::to_string() const
{
    switch(m_type)
    {
    case Type::Invalid:
        return "<invalid>";
    case Type::Null:
        return "null";
    case Type::Undefined:
        return "undefined";
    case Type::Int:
        return std::to_string(get_int());
    case Type::Bool:
        return get_bool() ? "true" : "false";
    case Type::Reference:
        return get_reference()->value().to_string();
    case Type::Object:
        return get_object()->to_string();
    default:
        assert(false);
        return {};
    }
}
    
std::shared_ptr<MemoryValue> Value::to_reference(Runtime& rt) const
{
    switch(m_type)
    {
    case Type::Null:
    case Type::Undefined:
    case Type::Int:
    case Type::Bool:
    case Type::Object:
        rt.throw_exception<Exception>("Cannot bind " + type_to_string(m_type) + " to reference");
        return {};
    case Type::Reference:
        return get_reference();
    default:
        assert(false);
        return {};
    }
}

std::shared_ptr<MemoryValue> Value::to_writable_reference(Runtime& rt) const
{
    auto value = to_reference(rt);
    if(!value)
        return value;
    if(value->is_read_only())
    {
        rt.throw_exception<Exception>("Cannot create writable reference for read-only object");
        return {};
    }
    return value;
}

std::string Value::dump_string() const
{
    std::ostringstream oss;
    oss << "Value{ " << type_to_string(m_type);
    switch(m_type)
    {
    case Type::Invalid:
    case Type::Null:
    case Type::Undefined:
        break;
    case Type::Int:
        oss << ": " << get_int();
        break;
    case Type::Bool:
        oss << ": " << (get_bool() ? "true" : "false");
        break;
    case Type::Reference:
        oss << ": " << get_reference()->dump_string();
        break;
    case Type::Object:
        oss << ": " << get_object()->type_name() << ":{" << get_object()->dump_string() + "}";
        break;
    default: assert(false);
    }
    oss << " }";
    return oss.str();
}

void Value::repl_print(std::ostream& output, bool print_members) const
{
    using namespace escapes;
    switch(m_type)
    {
    case Type::Invalid:
        output << error("<invalid>") << sgr_reset();
        break;
    case Type::Null:
        output << constant("null") << sgr_reset();
        break;
    case Type::Undefined:
        output << constant("undefined") << sgr_reset();
        break;
    case Type::Int:
        output << literal(std::to_string(get_int())) << sgr_reset();
        break;
    case Type::Bool:
        output << constant(get_bool() ? "true" : "false") << sgr_reset();
        break;
    case Type::Reference:
        get_reference()->value().repl_print(output, print_members);
        break;
    case Type::Object:
        get_object()->repl_print(output, print_members);
        break;
    default: assert(false);
    }
}

void Value::assign(Runtime& rt, Value const& other)
{
    if(is_reference())
    {
        auto memory_value = get_reference();
        if(memory_value->is_read_only())
        {
            rt.throw_exception<Exception>("Cannot assign to read-only object");
            return;
        }
        memory_value->value().assign(rt, other);
        return;
    }

    assign_direct(other.dereferenced());
}

void Value::assign_direct(Value const& other)
{
    m_type = other.m_type;
    switch(m_type)
    {
    case Type::Invalid:
    case Type::Null:
    case Type::Undefined:
        break;
    case Type::Int:
        m_value.integer = other.m_value.integer;
        break;
    case Type::Bool:
        m_value.boolean = other.m_value.boolean;
        break;
    case Type::Object:
        new(&m_value.object) std::shared_ptr<Object>(other.m_value.object);
        break;
    case Type::Reference:
        new(&m_value.reference) Reference(other.m_value.reference);
        break;
    default: assert(false);
    }
}

void Value::move_assign_direct(Value&& other)
{
    m_type = std::move(other.m_type);
    other.m_type = Type::Invalid;
    switch(m_type)
    {
    case Type::Invalid:
    case Type::Null:
    case Type::Undefined:
        break;
    case Type::Int:
        m_value.integer = std::exchange(other.m_value.integer, 0);
        break;
    case Type::Bool:
        m_value.boolean = std::exchange(other.m_value.boolean, 0);
        break;
    case Type::Object:
        new(&m_value.object) std::shared_ptr<Object>(std::move(other.m_value.object));
        break;
    case Type::Reference:
        new(&m_value.reference) Reference(std::move(other.m_value.reference));
        break;
    default: assert(false);
    }
}

Value Value::dereferenced() const
{
    return is_reference() ? get_reference()->value().dereferenced() : *this;
}

Value Value::call(Runtime& rt, ArgumentList const& arguments)
{
    Value real_value = dereferenced();
    if(!real_value.is_object())
    {
        rt.throw_exception<Exception>("Cannot call non-object");
        return {};
    }

    std::string name = is_reference() ? get_reference()->name() : "<anonymous>";
    auto container = this->container();
    ScopedExecutionContext context(rt, (is_reference() && container ? container->type_name() + "::" : "") + name + "()", container);
    if(rt.has_exception())
        return {}; // 'this' is not an object
    
    return real_value.get_object()->call(rt, container ? *container : *real_value.get_object(), arguments);
}

std::ostream& operator<<(std::ostream& stream, Value const& value)
{
    return stream << value.dump_string(); 
}

std::string MemoryValue::dump_string() const
{
    std::ostringstream oss;
    oss << "[ " << this << " = " << m_value << " ]";
    return oss.str();
}

void MemoryValue::repl_print(std::ostream& output, bool print_members) const
{
    if(is_read_only())
        output << escapes::comment("const") << " ";
    m_value.repl_print(output, print_members);
}

std::ostream& operator<<(std::ostream& stream, MemoryValue const& value)
{
    return stream << value.dump_string(); 
}

}
