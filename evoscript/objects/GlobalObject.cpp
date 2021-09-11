#include <evoscript/objects/GlobalObject.h>

namespace evo::script
{

GlobalObject::GlobalObject()
{
    DEFINE_NATIVE_OBJECT(object, "sys", std::make_shared<SysObject>());
}

}
