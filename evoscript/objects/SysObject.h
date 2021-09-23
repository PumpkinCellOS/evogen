#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class SysObject : public Object
{
public:
    SysObject();
    EVO_OBJECT("SysObject")

private:
    // Read a string from stdin until a newline.
    // function read() : string
    Value read(Runtime& rt, std::vector<Value> const& args);

    // Write objects (using to_string) to stdout, space-separated.
    // function write(values... : Value) : undefined
    Value write(Runtime& rt, std::vector<Value> const& args);

    Value backtrace(Runtime& rt, std::vector<Value> const& args);
};

}
