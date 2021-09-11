#pragma once

#include <evoscript/objects/Object.h>
#include <evoscript/objects/SysObject.h>

namespace evo::script
{

class GlobalObject : public Object
{
public:
    GlobalObject();

    virtual Value get(std::string const& member) override;

private:
    std::shared_ptr<SysObject> m_sys;
};

}
