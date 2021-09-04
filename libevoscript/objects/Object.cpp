#include <libevoscript/objects/Object.h>

#include <libevoscript/NativeFunction.h>
#include <libevoscript/Runtime.h>
#include <libevoscript/objects/StringObject.h>

#include <sstream>

namespace evo::script
{

Value Object::call(Runtime& rt, Object&, std::vector<Value> const&)
{
    rt.throw_exception("Cannot call non-callable object");
    return {};
}

Value Object::operator_add(Runtime& rt, Value const& rhs) const
{
    rt.throw_exception("Cannot call operator+ on object lhs=" + repl_string() + " with rhs=" + rhs.repl_string());
    return {};
}

CompareResult Object::operator_compare(Runtime& rt, Value const& rhs) const
{
    rt.throw_exception("Cannot call operator<> on object lhs=" + repl_string() + " with rhs=" + rhs.repl_string());
    return {};
}

Value Function::get(std::string const& member)
{
    NATIVE_OBJECT(object, "name", std::make_shared<StringObject>(m_name));
    return Value::undefined();
}

}
