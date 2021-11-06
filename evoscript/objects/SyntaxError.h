#pragma once

#include "evoscript/Object.h"
#include <evoscript/SourceLocation.h>
#include <evoscript/objects/Exception.h>

namespace evo::script
{

class SyntaxError : public Class, public NativeClass<SyntaxError>
{
    struct InternalData : public ObjectInternalData
    {
        std::string generated_source_display;

        InternalData(std::string const& source)
        : generated_source_display(source) {}
    };
public:
    SyntaxError();

    std::unique_ptr<ObjectInternalData> construct_internal_data(Runtime* rt, std::istream& input, ASTNode::ErrorList const& errors) const;

    virtual void print(Object const&, std::ostream& output, bool, bool) const override;

private:
    std::string m_message;
};

}
