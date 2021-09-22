#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <evoscript/StringId.h>

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
        Bool,
        Object,
        Reference
    };

    using IntType = int64_t;

    Value()
    : m_type(Type::Invalid) {}

    static std::string type_to_string(Type);

    Type type() const { return m_type; }
    std::string type_string() const { return type_to_string(type()); }

    static Value null() { return Value(Null); }
    static Value undefined() { return Value(Undefined); }
    static Value new_int(IntType value, std::shared_ptr<Object> const& container = {}) { return Value(value, container); }
    static Value new_bool(bool value, std::shared_ptr<Object> const& container = {}) { return Value(value, container); }
    static Value new_object(std::shared_ptr<Object> const& value, std::shared_ptr<Object> const& container = {}) { return Value(value, container); }
    static Value new_reference(std::shared_ptr<MemoryValue> const& value, std::shared_ptr<Object> const& container = {}) { return Value(value, container); }

    bool is_invalid() const { return m_type == Type::Invalid; }
    bool is_null() const { return m_type == Type::Null; }
    bool is_undefined() const { return m_type == Type::Undefined; }
    bool is_int() const { return m_type == Type::Int; }
    bool is_bool() const { return m_type == Type::Bool; }
    bool is_object() const { return m_type == Type::Object; }
    bool is_reference() const { return m_type == Type::Reference; }

    IntType to_int(Runtime&) const;
    bool to_bool(Runtime&) const;
    std::string to_string() const;
    std::shared_ptr<Object> to_object(Runtime&) const;
    std::shared_ptr<MemoryValue> to_reference(Runtime&) const;
    std::shared_ptr<MemoryValue> to_writable_reference(Runtime&) const;

    std::string dump_string() const;
    void repl_print(std::ostream& output, bool print_members) const;

    // This is type-unsafe and should be used only internally / by Runtime!
    IntType& get_int() { return m_int_value; }
    bool& get_bool() { return m_bool_value; }
    std::shared_ptr<Object>& get_object() { return m_object_value; }
    std::shared_ptr<MemoryValue>& get_reference() { assert(is_reference()); return m_reference_value; }

    IntType const& get_int() const { return m_int_value; }
    bool const& get_bool() const { return m_bool_value; }
    std::shared_ptr<Object> const& get_object() const { return m_object_value; }
    std::shared_ptr<MemoryValue> const& get_reference() const { return m_reference_value; }

    void assign(Runtime& rt, Value const& other);
    void assign_direct(Value const& other);

    Value dereferenced() const;

    Value call(Runtime&, std::vector<Value> const& arguments);

    std::shared_ptr<Object> container() const { return m_container; }
    void set_container(std::shared_ptr<Object> const& object) { m_container = object; }

    // TODO: Default version will probably not be optimal
    bool operator==(Value const&) const = default;

private:
    explicit Value(IntType value, std::shared_ptr<Object> const& container)
    : m_type(Type::Int), m_int_value(value), m_container(container) {}

    explicit Value(bool value, std::shared_ptr<Object> const& container)
    : m_type(Type::Bool), m_bool_value(value), m_container(container) {}

    explicit Value(std::shared_ptr<Object> const& value, std::shared_ptr<Object> const& container)
    : m_type(Type::Object), m_object_value(value), m_container(container)
        { assert(m_object_value); }

    explicit Value(std::shared_ptr<MemoryValue> const& value, std::shared_ptr<Object> const& container)
    : m_type(Type::Reference), m_reference_value(value), m_container(container)
        { assert(m_reference_value); }

    struct _UndefinedTag {}; static constexpr _UndefinedTag Undefined {};

    Value(_UndefinedTag)
    : m_type(Type::Undefined) {}

    struct _NullTag {}; static constexpr _NullTag Null {};

    Value(_NullTag)
    : m_type(Type::Null) {}

    Type m_type = Type::Null;

    IntType m_int_value = 0;
    bool m_bool_value = false;
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

    static std::shared_ptr<MemoryValue> create_int(Value::IntType value)
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

    StringId name() const { return m_name; }
    void set_name(StringId name) { m_name = name; }

    std::string dump_string() const;
    void repl_print(std::ostream& output, bool print_members) const;

private:
    Value m_value { Value::null() };
    bool m_read_only = false;
    StringId m_name;
};

std::ostream& operator<<(std::ostream&, MemoryValue const&);

}
