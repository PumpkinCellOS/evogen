#pragma once

#include <evoscript/AST.h>
#include <evoscript/Object.h>

namespace evo::script
{

class ASTFunction : public Class, public NativeClass<ASTFunction>
{
    struct InternalData : public ObjectInternalData
    {
        StringId name;
        std::shared_ptr<BlockStatement> body;
        std::vector<StringId> arg_names;

        InternalData(StringId name, std::shared_ptr<BlockStatement> const& body, std::vector<StringId> const& arg_names)
        : name(name), body(body), arg_names(arg_names) {}
    };

public:
    ASTFunction()
    : Class("ASTFunction") {}

    virtual std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, StringId name, std::shared_ptr<BlockStatement> const& body, std::vector<StringId> const& arg_names) const
    {
        return std::make_unique<InternalData>(name, body, arg_names);
    }

    virtual Value call(Runtime& rt, Object const& object, Object& this_, ArgumentList const& args) const override;
};

}
