#include "evoscript/EscapeSequences.h"
#include <evoscript/objects/Class.h>

#include <evoscript/NativeFunction.h>
#include <evoscript/objects/StringObject.h>

namespace evo::script
{

// TODO: Find a way to not store name twice
Class::Class(std::string const& name, ConstructorType&& constructor)
: m_name(name), m_constructor(constructor)
{
    DEFINE_NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name));
    define_native_function<Class>("construct", &Class::construct);
}

void Class::repl_print(std::ostream& output, bool print_members) const
{
    output << escapes::keyword("class") << " " << escapes::name(m_name) << " ";
    if(print_members)
        print_members_impl(output, false);
}

Value Class::construct(Runtime& rt, std::vector<Value> const& args)
{
    assert(m_constructor);
    return m_constructor(rt, args);
}

}
