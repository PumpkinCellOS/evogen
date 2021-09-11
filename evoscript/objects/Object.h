#pragma once

#include <evoscript/AbstractOperations.h>
#include <evoscript/Value.h>

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
    virtual std::string dump_string() const { return repl_string(); }
    // function repl_string() : string
    virtual std::string repl_string() const;
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

private:
    std::map<std::string, std::shared_ptr<MemoryValue>> m_values;
};

class Function : public Object
{
public:
    Function(std::string const& name)
    : m_name(name) {}

    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "Function"; }
    virtual std::string repl_string() const override { return "function " + m_name + "()"; }
    virtual std::string name() const override { return m_name; }

private:
    std::string m_name;
};

#define NATIVE_OBJECT(type, script_name, internal_name) \
    do {                                                \
        if(member == script_name)                       \
            return Value::new_##type(internal_name);    \
    } while(false)

}
