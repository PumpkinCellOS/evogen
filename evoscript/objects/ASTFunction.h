#pragma once

#include <evoscript/AST.h>
#include <evoscript/objects/Object.h>

namespace evo::script
{

class ASTFunction : public Function
{
public:
    ASTFunction(std::string const& name, std::shared_ptr<BlockStatement> const& body, std::vector<std::string> const& arg_names)
    : Function(name), m_body(body), m_arg_names(arg_names) {}

    virtual Value call(Runtime&, Object& container, std::vector<Value> const& arguments);

private:
    std::shared_ptr<BlockStatement> m_body;
    std::vector<std::string> m_arg_names;
};

}
