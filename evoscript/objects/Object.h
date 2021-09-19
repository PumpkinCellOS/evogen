#pragma once

#include <evoscript/AbstractOperations.h>
#include <evoscript/Value.h>

#include <sstream>

namespace evo::script
{

class Runtime;
class MemoryValue;

class Object
{
public:
    virtual Value get(std::string const& member);

    // TODO: Make these definiable functions, e.g:
    // function type() : string
    virtual std::string type_name() const { return "Object"; }
    // function dump_string() : string
    virtual std::string dump_string() const { std::ostringstream oss; repl_print(oss, true); return oss.str(); }
    // function repl_print() : string
    virtual void repl_print(std::ostream& output, bool print_members) const;
    // function to_string() : string
    virtual std::string to_string() const { return "[object " + type_name() + "]"; }
    // function call(container: Object, arguments: Array) : Value
    virtual Value call(Runtime&, Object& container, std::vector<Value> const& arguments);
    // function to_primitive() : Value
    virtual Value to_primitive(Runtime&, Value::Type) const { return {}; }
    // function operator+(rhs: Value) : Value
    virtual Value operator_add(Runtime& rt, Value const& rhs) const;
    // function operator<>(rhs: Value) : CompareResult
    virtual CompareResult operator_compare(Runtime& rt, Value const& rhs) const;
    // function name(): Value // this is already done for Function
    virtual std::string name() const { return ""; }

protected:
    void add_object_property(std::string const& name, std::shared_ptr<MemoryValue> memory_value)
    {
        m_values.insert(std::make_pair(name, memory_value));
    }

    template<class T, class... Args>
    void define_object_property(std::string const& name, Args&&... args)
    {
        auto memory_value = MemoryValue::create_object<T>(std::forward<Args>(args)...);
        add_object_property(name, memory_value);
    }

    template<class T, class... Args>
    void define_read_only_object_property(std::string const& name, Args&&... args)
    {
        auto memory_value = MemoryValue::create_object<T>(std::forward<Args>(args)...);
        memory_value->set_read_only(true);
        add_object_property(name, memory_value);
    }

private:
    std::map<std::string, std::shared_ptr<MemoryValue>> m_values;
};

class Function : public Object
{
public:
    Function(std::string const& name);

    virtual std::string type_name() const override { return "Function"; }
    virtual void repl_print(std::ostream& output, bool print_members) const override;
    virtual std::string name() const override { return m_name; }

private:
    virtual Value get(std::string const& member) override;

    std::string m_name;
};

#define NATIVE_OBJECT(type, script_name, internal_name) \
    do {                                                \
        if(member == script_name)                       \
            return Value::new_##type(internal_name);    \
    } while(false)

#define DEFINE_WRITABLE_NATIVE_OBJECT(type, script_name, internal_name) \
    add_object_property(script_name, std::make_shared<MemoryValue>(Value::new_##type(internal_name)));

#define DEFINE_NATIVE_OBJECT(type, script_name, internal_name)                     \
    do {                                                                                     \
        auto memory_value = std::make_shared<MemoryValue>(Value::new_##type(internal_name)); \
        memory_value->set_read_only(true);                                                   \
        add_object_property(script_name, memory_value);                                      \
    } while(false)

}

