#include <evoscript/objects/GlobalObject.h>

namespace evo::script
{

GlobalObject::GlobalObject()
{
    m_sys = std::make_shared<SysObject>();
}

Value GlobalObject::get(std::string const& member)
{
    NATIVE_OBJECT(object, "sys", m_sys);
    return Object::get(member);
}

}
