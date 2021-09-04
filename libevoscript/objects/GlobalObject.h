#pragma once

#include <libevoscript/objects/MapObject.h>
#include <libevoscript/objects/SysObject.h>

namespace evo::script
{

class GlobalObject : public MapObject
{
public:
    GlobalObject();

    virtual Value get(std::string const& member) override;

private:
    std::shared_ptr<SysObject> m_sys;
};

}
