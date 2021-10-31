#include <evoscript/objects/SyntaxError.h>

namespace evo::script
{

SyntaxError::SyntaxError(Runtime& rt, std::istream& input, ASTNode::ErrorList const& errors)
: Exception(rt, "SyntaxError")
{
    std::ostringstream oss;
    oss << "\e[1;31mSyntax Error\e[m" << std::endl;
    errors.print(oss, input);
    m_message = oss.str();
}

void SyntaxError::repl_print(std::ostream& output, bool) const
{
    output << m_message;
}

}
