#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <string>

namespace evo::script
{

class MemoryValue;
class Runtime;
class Value;

class Object
{
public:
    virtual Value get(std::string const& member) = 0;
    virtual std::string type_name() const { return "Object"; }
    virtual std::string dump_string() const { return ""; }
};

class MapObject : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "MapObject"; }
    virtual std::string dump_string() const override;

private:
    std::map<std::string, std::shared_ptr<MemoryValue>> m_values;
};


class Value
{
public:
    enum class Type
    {
        Invalid,
        Null,
        Undefined,
        Int,
        Object,
        Reference
    };

    Value()
    : m_type(Type::Invalid) {}

    static std::string type_to_string(Type);

    static Value null() { return Value(Null); }
    static Value undefined() { return Value(Undefined); }
    static Value new_int(int value) { return Value(value); }
    static Value new_object(std::shared_ptr<Object> value) { return Value(value); }
    static Value new_reference(std::shared_ptr<MemoryValue> value) { return Value(value); }

    bool is_null() const { return m_type == Type::Null; }
    bool is_undefined() const { return m_type == Type::Undefined; }
    bool is_int() const { return m_type == Type::Int; }
    bool is_object() const { return m_type == Type::Object; }
    bool is_reference() const { return m_type == Type::Reference; }
    
    std::string to_string(Runtime&) const;

    std::string dump_string() const;

    int to_int(Runtime&) const;
    std::shared_ptr<Object> to_object(Runtime&) const;
    std::shared_ptr<MemoryValue> to_reference(Runtime&) const;

    // This is type-unsafe and should be used only internally / by Runtime!
    int& get_int() { return m_int_value; }
    std::shared_ptr<Object>& get_object() { return m_object_value; }
    std::shared_ptr<MemoryValue>& get_reference() { return m_reference_value; }

    int const& get_int() const { return m_int_value; }
    std::shared_ptr<Object> const& get_object() const { return m_object_value; }
    std::shared_ptr<MemoryValue> const& get_reference() const { return m_reference_value; }

    void assign(Value const& other);
    void assign_direct(Value const& other);

    Value dereferenced() const;

private:
    explicit Value(int value)
    : m_type(Type::Int), m_int_value(value) {}

    explicit Value(std::shared_ptr<Object> value)
    : m_type(Type::Object), m_object_value(value) { assert(m_object_value); }

    explicit Value(std::shared_ptr<MemoryValue> value)
    : m_type(Type::Reference), m_reference_value(value) { assert(m_reference_value); }

    struct _UndefinedTag {}; static constexpr _UndefinedTag Undefined {};

    Value(_UndefinedTag)
    : m_type(Type::Undefined) {}

    struct _NullTag {}; static constexpr _NullTag Null {};

    Value(_NullTag)
    : m_type(Type::Null) {}

    Type m_type = Type::Null;

    int m_int_value = 0;
    std::shared_ptr<Object> m_object_value;
    std::shared_ptr<MemoryValue> m_reference_value;
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
