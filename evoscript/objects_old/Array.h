#pragma once

#include <evoscript/Value.h>
#include <evoscript/objects/Object.h>

namespace evo::script
{

class Array : public Object
{
public:
    Array();
    Array(Runtime&, ArgumentList const&);

    static void init_class(Class&);

    static std::shared_ptr<Array> from_argument_list(ArgumentList const&);
    static std::shared_ptr<Array> from_vector(std::vector<Value> const&);

    EVO_OBJECT("Array")

    virtual Value operator_subscript(Runtime& rt, Value const& rhs) override;
    virtual void repl_print(std::ostream&, bool print_members) const override;


private:
    std::vector<std::shared_ptr<MemoryValue>> m_values; 

    Value size(Runtime&, ArgumentList const& args) const;
};

}
