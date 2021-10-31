#pragma once

#include <evoscript/SourceLocation.h>
#include <evoscript/objects/Exception.h>

namespace evo::script
{

class SyntaxError : public Exception
{
public:
    SyntaxError(Runtime& rt, std::istream& input, ASTNode::ErrorList const& errors);

    EVO_OBJECT("SyntaxError")

    virtual void repl_print(std::ostream& output, bool) const override;

private:
    std::string m_message;
};

}
