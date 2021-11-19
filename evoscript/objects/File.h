#pragma once

#include <evoscript/Object.h>
#include <fstream>

namespace evo::script
{

// TODO: Access modes (write,append,text,binary,...)
class File : public Class, public NativeClass<File>
{
    struct InternalData : public ObjectInternalData
    {
        std::ifstream file;
    };
public:
    File();

    virtual void constructor(Runtime&, NativeObject<File>&, ArgumentList const&) const override;
    void constructor(Runtime&, NativeObject<File>&, std::string const& file_name) const;

private:
    static Value read(Runtime& rt, Object&, ArgumentList const& args);
    static Value close(Runtime& rt, Object&, ArgumentList const& args);
    static Value eof(Runtime& rt, Object&, ArgumentList const& args);

    std::ifstream m_file;
};

}
