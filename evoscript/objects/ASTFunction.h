#pragma once

#include <evoscript/AST.h>
#include <evoscript/objects/Object.h>

namespace evo::script
{

class ASTFunction : public Function
{
public:
    ASTFunction(StringId name, std::shared_ptr<BlockStatement> const& body, std::vector<StringId> const& arg_names)
    : Function(name), m_body(body), m_arg_names(arg_names) {}

    virtual Value call(Runtime&, Object& container, ArgumentList const& arguments);

private:
    std::shared_ptr<BlockStatement> m_body;
    std::vector<StringId> m_arg_names;
};

}
