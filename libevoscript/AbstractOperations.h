#pragma once

#include <libevoscript/Runtime.h>
#include <libevoscript/Value.h>

namespace evo::script::abstract
{

Value add(Runtime&, Value const& lhs, Value const& rhs);
Value subtract(Runtime&, Value const& lhs, Value const& rhs);
Value multiply(Runtime&, Value const& lhs, Value const& rhs);
Value divide(Runtime&, Value const& lhs, Value const& rhs);
Value modulo(Runtime&, Value const& lhs, Value const& rhs);

}
