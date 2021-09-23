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
    Value run_script(Runtime&, std::vector<Value> const& arguments);
};

}
