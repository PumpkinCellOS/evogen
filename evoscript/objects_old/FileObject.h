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

    FileObject(Runtime& rt, ArgumentList const& args)
    : FileObject(rt, args.get(0).to_string()) {}

    EVO_OBJECT("File")

private:
    Value read(Runtime& rt, ArgumentList const& args);
    Value close(Runtime& rt, ArgumentList const& args);
    Value eof(Runtime& rt, ArgumentList const& args);

    std::ifstream m_file;
};

}
