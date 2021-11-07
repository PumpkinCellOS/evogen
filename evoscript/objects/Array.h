#pragma once

#include <evoscript/Object.h>
#include <evoscript/Value.h>

namespace evo::script
{

class Array : public Class, public NativeClass<Array>
{
    struct InternalData : public ObjectInternalData
    {
        std::vector<std::shared_ptr<MemoryValue>> values; 
    };
public:
    Array();

    static void init_static_class_members(Object&);

    std::unique_ptr<InternalData> construct_internal_data(Runtime*) const
    {
        return std::make_unique<InternalData>();
    }

    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, ArgumentList const& args) const override;

    static std::shared_ptr<Object> from_argument_list(ArgumentList const&);
    static std::shared_ptr<Object> from_vector(std::vector<Value> const&);

    virtual Value operator_subscript(Runtime& rt, Object&, Value const& rhs) const override;
    virtual void print(Object const&, std::ostream&, bool print_members, bool dump) const override;

private:
    static Value size(Runtime&, Object&, ArgumentList const& args);
};

}
