#pragma once

#include <evoscript/ScopeObject.h>

namespace evo::script
{

class GlobalObject : public ScopeObject
{
public:
    GlobalObject(Runtime& rt);

private:
    static Value run_script(Runtime&, Object&, ArgumentList const& arguments);
};

}
