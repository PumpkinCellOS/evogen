#include <libevoscript/SourceLocation.h>

namespace evo::script
{

std::ostream& operator<<(std::ostream& stream, SourceLocation const& node)
{
    return stream << node.line << ":" << node.column;
}

}
