#pragma once

#include <libevoscript/Value.h>

namespace evo::script
{

enum class CompareResult
{
    Equal,
    Less,
    Greater,
    Unknown
};

class Runtime;

namespace abstract
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

CompareResult compare(Runtime& rt, Value const& lhs, Value const& rhs);

Value postfix_increment(Runtime&, Value const&);
Value postfix_decrement(Runtime&, Value const&);

// Requires a reference!
Value prefix_increment(Runtime&, Value const&);
Value prefix_decrement(Runtime&, Value const&);

}

}
