#include <libevoscript/Value.h>

#include <libevoscript/Runtime.h>

#include <cassert>
#include <sstream>

namespace evo::script
{

Value Object::call(Runtime& rt)
{
    rt.throw_exception("Cannot call non-callable object");
    return {};
}

Value MapObject::get(std::string const& member)
{
    if(member == "length")
    {
        return Value::new_object(std::make_shared<NativeFunction>([](Runtime& rt)->Value {
            return Value::new_int(rt.current_execution_context().this_object<MapObject>()->m_values.size());
        }));
    }

    auto it = m_values.find(member);
    if(it == m_values.end())
    {
        // Reference to newly-created undefined.
        // TODO: Tidy up these undefineds after exiting an expression
        return Value::new_reference(m_values.insert(std::make_pair(member, MemoryValue::create_undefined())).first->second);
    }

    return Value::new_reference(it->second);
}

std::string MapObject::dump_string() const
{
    std::ostringstream oss;
    for(auto& value: m_values)
    {
        oss << value.first << ": " << value.second->dump_string() << ", ";
    }
    return oss.str();
}

Value Function::get(std::string const&)
{
    return Value::undefined();
}

Value NativeFunction::call(Runtime& rt)
{
    assert(m_function);
    return m_function(rt);
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
    case Type::Object:
        assert(m_object_value);
        return m_object_value;
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_object(rt);
    default: assert(false);
    }
}

std::string Value::to_string(Runtime& rt) const
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
    case Type::Reference:
        assert(m_reference_value);
        return m_reference_value->value().to_string(rt);
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

Value Value::call(Runtime& rt)
{
    if(!is_object())
    {
        rt.throw_exception("Cannot call non-object");
        return {};
    }
    return get_object()->call(rt);
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
