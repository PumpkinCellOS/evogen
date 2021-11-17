#pragma once

#include <evoscript/Object.h>
#include <evoscript/Value.h>

namespace evo::script
{

class Array : public Class, public NativeClass<Array>
{
public:
    struct InternalData : public ObjectInternalData
    {
        std::vector<std::shared_ptr<MemoryValue>> values; 
    };

    Array();

    static void init_static_class_members(Object&);

    void constructor(Runtime&, NativeObject<Array>& object) const {}
    virtual void constructor(Runtime&, NativeObject<Array>& object, ArgumentList const&) const override;

    static std::shared_ptr<Object> from_argument_list(ArgumentList const&);
    static std::shared_ptr<Object> from_vector(std::vector<Value> const&);

private:
    static Value size(Runtime&, NativeObject<Array>&, ArgumentList const& args);
};

}
