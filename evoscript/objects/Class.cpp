#include "evoscript/NativeFunction.h"
#include <evoscript/objects/Class.h>

namespace evo::script
{

Class::Class(ConstructorType&& constructor)
: m_constructor(constructor)
{
    DEFINE_NATIVE_FUNCTION(Class, "construct", construct);
}

Value Class::construct(Runtime& rt, Class& class_, std::vector<Value> const& args)
{
    assert(class_.m_constructor);
    return class_.m_constructor(rt, args);
}

}
