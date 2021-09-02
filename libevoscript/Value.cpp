#include <libevoscript/Value.h>

#include <libevoscript/NativeFunction.h>
#include <libevoscript/Runtime.h>
#include <libevoscript/objects/Object.h>
#include <libevoscript/objects/StringObject.h>

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
    case Type::String:
        return "string";
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
    case Type::Object:
        // TODO: Allow conversion from objects
        rt.throw_exception("Cannot convert " + type_to_string(m_type) + " to int");
        return 0;
    case Type::Int:
        return m_int_value;
    case Type::String:
        try
        {
            return std::stoi(m_string_value);
        }
        catch(...)
        {
            rt.throw_exception("Cannot convert string '" + m_string_value + "' to int");
            return {};
        }
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_int(rt);
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
        rt.throw_exception("Cannot convert " + type_to_string(m_type) + " to object");
        return nullptr;
    case Type::String:
        return std::make_shared<StringObject>(m_string_value);
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
    case Type::String:
        return m_string_value;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_string();
    case Type::Object:
        assert(m_object_value);
        return "[object " + m_object_value->type_name() + "]";
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
    case Type::String:
    case Type::Object:
        rt.throw_exception("Cannot bind " + type_to_string(m_type) + " to reference");
        return nullptr;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value;
    default: assert(false);
    }
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
    case Type::String:
        oss << ": \"" << m_string_value << "\"";
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

std::string Value::repl_string() const
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
    case Type::String:
        return "\"" + m_string_value + "\"";
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().repl_string();
    case Type::Object:
        assert(m_object_value);
        return m_object_value->repl_string();
    default: assert(false);
    }
}

void Value::assign(Value const& other)
{
    if(is_reference())
    {
        assert(m_reference_value);
        get_reference()->value().assign(other);
        return;
    }

    Value real_other = other.dereferenced();
    assert(!real_other.is_reference());
    switch(real_other.m_type)
    {
    case Type::Invalid:
    case Type::Null:
    case Type::Undefined:
        break;
    case Type::Int:
        m_int_value = real_other.m_int_value;
        break;
    case Type::String:
        m_string_value = real_other.m_string_value;
        break;
    case Type::Object:
        assert(real_other.m_object_value);
        m_object_value = real_other.m_object_value;
        break;
    // References are handled at the beginning and
    // for real_other. They should not appear here.
    default: assert(false);
    }
    m_type = real_other.m_type;
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
    case Type::String:
        m_string_value = other.m_string_value;
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
    assert(m_container);
    return real_value.get_object()->call(rt, *m_container, arguments);
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

std::ostream& operator<<(std::ostream& stream, MemoryValue const& value)
{
    return stream << value.dump_string(); 
}

}
