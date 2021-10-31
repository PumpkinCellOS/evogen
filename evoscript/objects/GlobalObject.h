#pragma once

#include <evoscript/objects/Object.h>
#include <evoscript/objects/ScopeObject.h>
#include <evoscript/objects/SysObject.h>

namespace evo::script
{

class GlobalObject : public ScopeObject
{
public:
    GlobalObject();

    EVO_OBJECT("GlobalObject")

private:
    Value run_script(Runtime&, ArgumentList const& arguments);
};

}
