#pragma once

#include <evoscript/Object.h>

namespace evo::script
{

class System : public Class, public NativeClass<System>
{
public:
    System();

private:
    // Read a string from stdin until a newline.
    // function read() : String
    static Value read(Runtime& rt, Object&, ArgumentList const& args);

    // Write objects (using to_string) to stdout.
    // function write(values... : Value) : undefined
    static Value write(Runtime& rt, Object&, ArgumentList const& args);

    // Write objects (using to_string) to stdout with newline appended.
    // function writeln(values... : Value) : undefined
    static Value writeln(Runtime& rt, Object&, ArgumentList const& args);

    // Print a backtrace.
    // function backtrace() : undefined
    static Value backtrace(Runtime& rt, Object&, ArgumentList const& args);

    // Run a shell command. Equivalent to C's `system` function.
    // function call_system(command : String) : int
    static Value call_system(Runtime& rt, Object&, ArgumentList const& args);

    // Get current working directory.
    // function cwd() : String
    static Value cwd(Runtime& rt, Object&, ArgumentList const& args);
};

}
