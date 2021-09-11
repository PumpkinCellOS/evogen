#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class SysObject : public Object
{
public:
    virtual Value get(std::string const& member) override;
    virtual std::string type_name() const override { return "SysObject"; }

private:
    // Read a string from stdin until a newline.
    // function read() : string
    static Value read(Runtime& rt, SysObject& container, std::vector<Value> const& args);

    // Write objects (using to_string) to stdout, space-separated.
    // function write(values... : Value) : undefined
    static Value write(Runtime& rt, SysObject& container, std::vector<Value> const& args);

    static Value backtrace(Runtime& rt, SysObject& container, std::vector<Value> const& args);
};

}
