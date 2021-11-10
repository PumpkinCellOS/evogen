#pragma once

#include <evoscript/Object.h>

namespace evo::script
{

class FileSystem : public Class, public NativeClass<FileSystem>
{
public:
    FileSystem();

private:
    static Value list_files(Runtime& rt, Object& object, ArgumentList const& args);
};

}
