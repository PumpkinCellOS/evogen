#pragma once

#include <evoscript/objects/Object.h>
#include <evoscript/objects/SysObject.h>

namespace evo::script
{

class GlobalObject : public Object
{
public:
    GlobalObject();

private:
    static Value run_script(Runtime&, GlobalObject&, std::vector<Value> const& arguments);
};

}
