#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <evoscript/StringId.h>

namespace evo::script
{

class ArgumentList;
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

    Value(Value const& other) { *this = other; }
    Value(Value&& other) { *this = std::move(other); }

    Value& operator=(Value const&);
    Value& operator=(Value&&);
    ~Value();

    static std::string type_to_string(Type);

    Type type() const { return m_type; }
    std::string type_string() const { return type_to_string(type()); }

    static Value null() { return Value(Null); }
    static Value undefined() { return Value(Undefined); }
    static Value new_int(IntType value) { return Value(value); }
    static Value new_bool(bool value) { return Value(value); }
    static Value new_object(std::shared_ptr<Object> const& value) { return Value(value); }
    static Value new_reference(std::shared_ptr<MemoryValue> const& value, Object* container) { return Value(value, container); }

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
    IntType& get_int() { return m_value.integer; }
    bool& get_bool() { return m_value.boolean; }
    std::shared_ptr<Object>& get_object() { return m_value.object; }
    std::shared_ptr<MemoryValue>& get_reference() { assert(is_reference()); return m_value.reference.value; }

    IntType const& get_int() const { return m_value.integer; }
    bool const& get_bool() const { return m_value.boolean; }
    std::shared_ptr<Object> const& get_object() const { return m_value.object; }
    std::shared_ptr<MemoryValue> const& get_reference() const { assert(is_reference()); return m_value.reference.value; }

    void assign(Runtime& rt, Value const& other);
    void assign_direct(Value const& other);

    Value const& dereferenced() const;

    Value call(Runtime&, ArgumentList const&);

    Object* container() const { assert(is_reference()); return m_value.reference.container; }

private:
    explicit Value(IntType value)
    : m_type(Type::Int) { m_value.integer = value; }

    explicit Value(bool value)
    : m_type(Type::Bool) { m_value.boolean = value; }

    explicit Value(std::shared_ptr<Object> const& value)
    : m_type(Type::Object)
        { assert(value); new(&m_value.object) std::shared_ptr<Object>(value); }

    explicit Value(std::shared_ptr<MemoryValue> const& value, Object* container)
    : m_type(Type::Reference)
        { assert(value); new(&m_value.reference) Reference({value, container}); }

    struct _UndefinedTag {}; static constexpr _UndefinedTag Undefined {};

    Value(_UndefinedTag)
    : m_type(Type::Undefined) {}

    struct _NullTag {}; static constexpr _NullTag Null {};

    Value(_NullTag)
    : m_type(Type::Null) {}

    Type m_type = Type::Null;

    struct Reference
    {
        std::shared_ptr<MemoryValue> value;
        Object* container { nullptr };
    };

    union InternalValue
    {
        IntType integer;
        bool boolean;
        std::shared_ptr<Object> object;
        Reference reference;

        // This needs to be handled in Value itself
        InternalValue() {}
        ~InternalValue() {}
    } m_value;

    void move_assign_direct(Value&& other);
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

    static std::shared_ptr<MemoryValue> create_object(std::shared_ptr<Object> value)
    {
        return std::make_shared<MemoryValue>(Value::new_object(value));
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
