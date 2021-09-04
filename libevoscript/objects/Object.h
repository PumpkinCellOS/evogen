#pragma once

#include <libevoscript/Value.h>

namespace evo::script
{

class Runtime;
class MemoryValue;

class Object
{
public:
    virtual Value get(std::string const& member) = 0;

    // TODO: Make these definiable functions, e.g:
    // function type() : string
    virtual std::string type_name() const { return "Object"; }
    // function dump_string() : string
    virtual std::string dump_string() const { return repl_string(); }
    // function repl_string() : string
    virtual std::string repl_string() const { return type_name() + " {}"; }
    // function call(container: Object, arguments: Array) : Value
    virtual Value call(Runtime&, Object& container, std::vector<Value> const& arguments);
    // function to_primitive() : Value
    virtual Value to_primitive(Runtime&, Value::Type) const { return {}; }
    // function operator+(rhs: Value) : Value
    virtual Value operator_add(Runtime& rt, Value const& rhs) const;
};

class Function : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "Function"; }
    virtual std::string repl_string() const override { return "function() {}"; }
};

#define NATIVE_OBJECT(type, script_name, internal_name) \
    do {                                                \
        if(member == script_name)                       \
            return Value::new_##type(internal_name);    \
    } while(false)

}
