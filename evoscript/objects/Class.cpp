#include "evoscript/EscapeSequences.h"
#include <evoscript/objects/Class.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/StringObject.h>

namespace evo::script
{

// TODO: Find a way to not store name double
Class::Class(std::string const& name, ConstructorType&& constructor)
: m_name(name), m_constructor(constructor)
{
    DEFINE_NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name));
    DEFINE_NATIVE_FUNCTION(Class, "construct", construct);
}

void Class::repl_print(std::ostream& output, bool) const
{
    output << escapes::keyword("class") << " " << escapes::name(m_name);
}

Value Class::construct(Runtime& rt, Class& class_, std::vector<Value> const& args)
{
    assert(class_.m_constructor);
    return class_.m_constructor(rt, args);
}

}
