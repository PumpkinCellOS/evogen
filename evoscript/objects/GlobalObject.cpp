#include <evoscript/objects/GlobalObject.h>

#include <evoscript/objects/Class.h>
#include <evoscript/objects/Exception.h>
#include <evoscript/objects/StringObject.h>

namespace evo::script
{

GlobalObject::GlobalObject()
{
    DEFINE_NATIVE_OBJECT(object, "sys", std::make_shared<SysObject>());

    DEFINE_NATIVE_OBJECT(object, "Object", create_native_class<Object>());
    DEFINE_NATIVE_OBJECT(object, "String", create_native_class<StringObject>());
    DEFINE_NATIVE_OBJECT(object, "Exception", create_native_class<Exception>());
}

}
