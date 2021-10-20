#pragma once

#include <evoscript/objects/Object.h>
#include <fstream>

namespace evo::script
{

// TODO: Access modes (write,append,text,binary,...)
class FileObject : public Object
{
public:
    FileObject(Runtime& rt, std::string const& name);

    FileObject(Runtime& rt, std::vector<Value> const& args)
    : FileObject(rt, args.empty() ? "" : args[0].to_string()) {}

    EVO_OBJECT("File")

private:
    Value read(Runtime& rt, std::vector<Value> const& args);
    Value close(Runtime& rt, std::vector<Value> const& args);
    Value eof(Runtime& rt, std::vector<Value> const& args);

    std::ifstream m_file;
};

}
