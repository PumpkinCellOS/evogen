#pragma once

#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <memory>

namespace evo::script
{

class String : public Class, public NativeClass<String>
{
public:
    struct InternalData : public ObjectInternalData
    {
        std::string string;

        InternalData(std::string const& string_)
        : string(string_) {}
    };

    String();

    void constructor(Runtime&, NativeObject<String>& object, std::string const& value) const
    {
        object.internal_data().string = value;
    }

    virtual void constructor(Runtime& rt, NativeObject<String>& object, ArgumentList const& args) const override
    {
        constructor(rt, object, args.get(0).to_string());
    }

    static void init_static_class_members(Object& class_);
};

}
