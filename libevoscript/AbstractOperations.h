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

Value not_(Runtime&, Value const&);
Value bitwise_not(Runtime&, Value const&);
Value minus(Runtime&, Value const&);
Value plus(Runtime&, Value const&);

// Requires a reference!
Value prefix_increment(Runtime&, Value const&);
Value prefix_decrement(Runtime&, Value const&);

}
