#include <evoscript/objects/SyntaxError.h>

namespace evo::script
{

void SyntaxError::repl_print(std::ostream& output, bool print_members) const
{
    std::cout << "\e[1;31mSyntax Error\e[m" << std::endl;
    m_errors.print(output, m_input);
}

}
