#pragma once

#include <evoscript/Object.h>
#include <evoscript/Runtime.h>
#include <memory>

namespace evo::script
{

class String : public Class, public NativeClass<String>
{
    struct InternalData : public ObjectInternalData
    {
        std::string string;

        InternalData(std::string const& string_)
        : string(string_) {}
    };
public:
    String();

    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime*, std::string const& value) const
    {
        return std::make_unique<InternalData>(value);
    }

    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, ArgumentList const& args) const override
    {
        return construct_internal_data(rt, args.get(0).to_string());
    }

    static void init_static_class_members(Object& class_);

    virtual void print(Object const&, std::ostream&, bool print_members, bool dump) const override;
    virtual std::string to_string(Object const&) const override;
    virtual Value to_primitive(Runtime&, Object const&, Value::Type) const override;
    virtual Value operator_add(Runtime& rt, Object const&, Value const& rhs) const override;
    virtual CompareResult operator_compare(Runtime& rt, Object const&, Value const& rhs) const override;
    virtual Value operator_subscript(Runtime& rt, Object&, Value const& rhs) const override;

private:
    static Value concat(Runtime& rt, Object const&, ArgumentList const& args);
    static Value substring(Runtime& rt, Object const& object, ArgumentList const& args);
};

}
