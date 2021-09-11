#pragma once

#include <evoscript/AST.h>
#include <evoscript/objects/Object.h>

namespace evo::script
{

class ASTFunction : public Function
{
public:
    ASTFunction(std::string const& name, std::shared_ptr<BlockStatement> const& body)
    : Function(name), m_body(body) {}

    virtual Value call(Runtime&, Object& container, std::vector<Value> const& arguments);

private:
    std::shared_ptr<BlockStatement> m_body;
};

}
