#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace evo::script
{

class MemoryValue;
class Object;
class Runtime;

class Value
{
public:
    enum class Type
    {
        Invalid,
        Null,
        Undefined,
        Int,
        String,
        Object,
        Reference
    };

    Value()
    : m_type(Type::Invalid) {}

    static std::string type_to_string(Type);

    Type type() const { return m_type; }
    std::string type_string() const { return type_to_string(type()); }

    static Value null() { return Value(Null); }
    static Value undefined() { return Value(Undefined); }
    static Value new_int(int value, std::shared_ptr<Object> container = {}) { return Value(value, container); }
    static Value new_string(std::string const& value, std::shared_ptr<Object> container = {}) { return Value(value, container); }
    static Value new_object(std::shared_ptr<Object> value, std::shared_ptr<Object> container = {}) { return Value(value, container); }
    static Value new_reference(std::shared_ptr<MemoryValue> value, std::shared_ptr<Object> container = {}) { return Value(value, container); }

    bool is_invalid() const { return m_type == Type::Invalid; }
    bool is_null() const { return m_type == Type::Null; }
    bool is_undefined() const { return m_type == Type::Undefined; }
    bool is_int() const { return m_type == Type::Int; }
    bool is_string() const { return m_type == Type::String; }
    bool is_object() const { return m_type == Type::Object; }
    bool is_reference() const { return m_type == Type::Reference; }

    int to_int(Runtime&) const;
    std::string to_string(Runtime&) const;
    std::shared_ptr<Object> to_object(Runtime&) const;
    std::shared_ptr<MemoryValue> to_reference(Runtime&) const;

    std::string dump_string() const;

    // This is type-unsafe and should be used only internally / by Runtime!
    int& get_int() { return m_int_value; }
    std::string& get_string() { return m_string_value; }
    std::shared_ptr<Object>& get_object() { return m_object_value; }
    std::shared_ptr<MemoryValue>& get_reference() { return m_reference_value; }

    int const& get_int() const { return m_int_value; }
    std::string const& get_string() const { return m_string_value; }
    std::shared_ptr<Object> const& get_object() const { return m_object_value; }
    std::shared_ptr<MemoryValue> const& get_reference() const { return m_reference_value; }

    void assign(Value const& other);
    void assign_direct(Value const& other);

    Value dereferenced() const;

    Value call(Runtime&, std::vector<Value> const& arguments);
    std::shared_ptr<Object> container() const { return m_container; }
    void set_container(std::shared_ptr<Object> object) { m_container = object; }

private:
    explicit Value(int value, std::shared_ptr<Object> container)
    : m_type(Type::Int), m_int_value(value), m_container(container) {}

    explicit Value(std::string const& value, std::shared_ptr<Object> container)
    : m_type(Type::String), m_string_value(value), m_container(container) {}

    explicit Value(std::shared_ptr<Object> value, std::shared_ptr<Object> container)
    : m_type(Type::Object), m_object_value(value), m_container(container)
        { assert(m_object_value); }

    explicit Value(std::shared_ptr<MemoryValue> value, std::shared_ptr<Object> container)
    : m_type(Type::Reference), m_reference_value(value), m_container(container)
        { assert(m_reference_value); }

    struct _UndefinedTag {}; static constexpr _UndefinedTag Undefined {};

    Value(_UndefinedTag)
    : m_type(Type::Undefined) {}

    struct _NullTag {}; static constexpr _NullTag Null {};

    Value(_NullTag)
    : m_type(Type::Null) {}

    Type m_type = Type::Null;

    int m_int_value = 0;
    std::string m_string_value;
    std::shared_ptr<Object> m_object_value;
    std::shared_ptr<MemoryValue> m_reference_value;
    std::shared_ptr<Object> m_container;
};

std::ostream& operator<<(std::ostream&, Value const&);

class MemoryValue
{
public:
    MemoryValue() = default;

    MemoryValue(Value const& value)
    : m_value(value) {}

    static std::shared_ptr<MemoryValue> create_null()
    {
        return std::make_shared<MemoryValue>(Value::null());
    }

    static std::shared_ptr<MemoryValue> create_undefined()
    {
        return std::make_shared<MemoryValue>(Value::undefined());
    }

    static std::shared_ptr<MemoryValue> create_int(int value)
    {
        return std::make_shared<MemoryValue>(Value::new_int(value));
    }

    static std::shared_ptr<MemoryValue> create_existing_object(std::shared_ptr<Object> value)
    {
        return std::make_shared<MemoryValue>(Value::new_object(value));
    }

    template<class T, class... Args>
    static std::shared_ptr<MemoryValue> create_object(Args&&... args)
    {
        return std::make_shared<MemoryValue>(Value::new_object(std::make_shared<T>(std::forward<Args>(args)...)));
    }

    Value& value() { return m_value; }
    Value const& value() const { return m_value; }

    bool is_read_only() const { return m_read_only; }
    void set_read_only(bool read_only) { m_read_only = read_only; }

    std::string dump_string() const;

private:
    Value m_value { Value::null() };
    bool m_read_only = false;
};

std::ostream& operator<<(std::ostream&, MemoryValue const&);

}
