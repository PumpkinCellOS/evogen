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
    // function read() : String
    Value read(Runtime& rt, ArgumentList const& args);

    // Write objects (using to_string) to stdout.
    // function write(values... : Value) : undefined
    Value write(Runtime& rt, ArgumentList const& args);

    // Write objects (using to_string) to stdout with newline appended.
    // function writeln(values... : Value) : undefined
    Value writeln(Runtime& rt, ArgumentList const& args);

    // Print a backtrace.
    // function backtrace() : undefined
    Value backtrace(Runtime& rt, ArgumentList const& args);

    // Run a shell command. Equivalent to C's `system` function.
    // function call_system(command : String) : int
    Value call_system(Runtime& rt, ArgumentList const& args);

    // Get current working directory.
    // function cwd() : String
    Value cwd(Runtime& rt, ArgumentList const& args);
};

}
