#pragma once

#include <evoscript/ScopeObject.h>

namespace evo::script
{

class GlobalObject : public ScopeObject
{
public:
    GlobalObject();

private:
    static Value run_script(Runtime&, Object&, ArgumentList const& arguments);
};

}
