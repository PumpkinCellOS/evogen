#pragma once

#include <evoscript/objects/Object.h>

namespace evo::script
{

class FsObject : public Object
{
public:
    FsObject();
    EVO_OBJECT("FsObject")

private:
    Value list_files(Runtime& rt, ArgumentList const& args) const;
};

}
