#pragma once

#include <evoscript/Value.h>

namespace evo::script
{

class EvalResult
{
public:
    enum CompletionType
    {
        Normal,
        Return
    };

    EvalResult(Value const& value = {})
    : EvalResult(value, Normal) {}

    static EvalResult return_(Value const& value = {}) { return EvalResult(value, Return); }

    CompletionType type() const { return m_type; }
    Value value() const { return m_value; }
    operator Value() const { return value(); }

    bool is_normal() const { return m_type == Normal; }
    bool is_return() const { return m_type == Return; }

    bool is_abrupt() const { return !is_normal(); }

private:
    EvalResult(Value const& value, CompletionType type)
    : m_value(value), m_type(type) {}

    Value m_value;
    CompletionType m_type;
};

}
