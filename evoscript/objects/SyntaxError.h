#pragma once

#include <evoscript/SourceLocation.h>
#include <evoscript/objects/Exception.h>

namespace evo::script
{

class SyntaxError : public Exception
{
public:
    SyntaxError(Runtime& rt, std::istream& input, ASTNode::ErrorList const& errors)
    : Exception(rt, "Syntax Error"), m_errors(errors), m_input(input) {}

    EVO_OBJECT("SyntaxError")

    virtual void repl_print(std::ostream& output, bool) const override;

private:
    ASTNode::ErrorList m_errors;
    std::istream& m_input;
};

}
