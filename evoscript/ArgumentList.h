#pragma once

#include <evoscript/Value.h>
#include <vector>

namespace evo::script
{

class ArgumentList : std::vector<Value>
{
public:
    ArgumentList() = default;

    explicit ArgumentList(std::vector<Value> const& other)
    : std::vector<Value>(other) {}

    std::vector<Value> values() const { return *this; }
    Value get(size_t index) const { return index < size() ? at(index) : Value::undefined(); }
    Value get_or(size_t index, Value const& alternative) const { return is_given(index) ? at(index) : alternative; }
    bool is_given(size_t index) const { return index < size(); }
    bool is_defined(size_t index) const { return is_given(index) && !at(index).is_undefined(); }
    size_t size() const { return vector::size(); }

    const_iterator begin() const { return vector::begin(); }
    const_iterator end() const { return vector::end(); }
};

}
