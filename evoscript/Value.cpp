#include <evoscript/Value.h>

#include <evoscript/EscapeSequences.h>
#include <evoscript/NativeFunction.h>
#include <evoscript/Runtime.h>
#include <evoscript/objects/Object.h>
#include <evoscript/objects/StringObject.h>

#include <cassert>
#include <sstream>

namespace evo::script
{

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
    
int Value::to_int(Runtime& rt) const
{
    switch(m_type)
    {
    case Type::Null:
    case Type::Undefined:
        return 0;
    case Type::Object:
    {
        assert(m_object_value);
        auto primitive = m_object_value->to_primitive(rt, Value::Type::Int);
        if(rt.has_exception())
            return 0;
        return primitive.to_int(rt);
    }
    case Type::Int:
        return m_int_value;
    case Type::Bool:
        return m_bool_value ? 1 : 0;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_int(rt);
    default:
        assert(false);
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
        return m_int_value != 0;
    case Type::Bool:
        return m_bool_value;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_bool(rt);
    default:
        assert(false);
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
        rt.throw_exception("Cannot convert " + type_to_string(m_type) + " to object");
        return nullptr;
    case Type::Object:
        assert(m_object_value);
        return m_object_value;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_object(rt);
    default: assert(false);
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
        return std::to_string(m_int_value);
    case Type::Bool:
        return m_bool_value ? "true" : "false";
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_string();
    case Type::Object:
        assert(m_object_value);
        return m_object_value->to_string();
    default: assert(false);
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
        rt.throw_exception("Cannot bind " + type_to_string(m_type) + " to reference");
        return {};
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value;
    default: assert(false);
    }
}

std::shared_ptr<MemoryValue> Value::to_writable_reference(Runtime& rt) const
{
    auto value = to_reference(rt);
    if(!value)
        return value;
    if(value->is_read_only())
    {
        rt.throw_exception("Cannot create writable reference for read-only object");
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
        oss << ": " << m_int_value;
        break;
    case Type::Bool:
        oss << ": " << (m_bool_value ? "true" : "false");
        break;
    case Type::Reference:
        assert(m_reference_value);
        oss << ": " << m_reference_value->dump_string();
        break;
    case Type::Object:
        assert(m_object_value);
        oss << ": " << m_object_value->type_name() << ":{" << m_object_value->dump_string() + "}";
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
        output << literal(std::to_string(m_int_value)) << sgr_reset();
        break;
    case Type::Bool:
        output << constant(m_bool_value ? "true" : "false") << sgr_reset();
        break;
    case Type::Reference:
        assert(m_reference_value);
        m_reference_value->value().repl_print(output, print_members);
        break;
    case Type::Object:
        assert(m_object_value);
        m_object_value->repl_print(output, print_members);
        break;
    default: assert(false);
    }
}

void Value::assign(Runtime& rt, Value const& other)
{
    if(is_reference())
    {
        assert(m_reference_value);
        auto memory_value = get_reference();
        if(memory_value->is_read_only())
        {
            rt.throw_exception("Cannot assign to read-only object");
            return;
        }
        memory_value->value().assign(rt, other);
        return;
    }

    assign_direct(other.dereferenced());
}

void Value::assign_direct(Value const& other)
{
    if(is_reference())
    {
        assert(m_reference_value);
        get_reference()->value().assign_direct(other);
        return;
    }

    m_type = other.m_type;
    switch(m_type)
    {
    case Type::Invalid:
    case Type::Null:
    case Type::Undefined:
        break;
    case Type::Int:
        m_int_value = other.m_int_value;
        break;
    case Type::Bool:
        m_bool_value = other.m_bool_value;
        break;
    case Type::Object:
        assert(other.m_object_value);
        m_object_value = other.m_object_value;
        break;
    case Type::Reference:
        assert(other.m_reference_value);
        m_reference_value = other.m_reference_value;
        break;
    default: assert(false);
    }
}

Value Value::dereferenced() const
{
    return is_reference() ? get_reference()->value().dereferenced() : *this;
}

Value Value::call(Runtime& rt, std::vector<Value> const& arguments)
{
    Value real_value = dereferenced();
    if(!real_value.is_object())
    {
        rt.throw_exception("Cannot call non-object");
        return {};
    }

    ScopedExecutionContext context(rt, (m_container ? m_container->type_name() + "::" : "") + name() + "()", container());
    if(rt.has_exception())
        return {}; // 'this' is not an object
    
    return real_value.get_object()->call(rt, m_container ? *m_container : *real_value.get_object(), arguments);
}

std::string Value::name() const
{
    auto real_value = dereferenced();
    if(!real_value.is_object())
        return "?";
    auto name = real_value.get_object()->name();
    if(name.empty())
    {
        if(!m_name.empty())
            return m_name;
        return "<object>";
    }
    return name;
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
    m_value.repl_print(output, print_members);
}

std::ostream& operator<<(std::ostream& stream, MemoryValue const& value)
{
    return stream << value.dump_string(); 
}

}
