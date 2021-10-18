#include <evoscript/SourceLocation.h>
#include <istream>

namespace evo::script
{

std::ostream& operator<<(std::ostream& stream, SourceLocation const& node)
{
    return stream << node.line << ":" << node.column;
}

void display_source_range(std::ostream& output, std::istream& input, SourceSpan const& span)
{
    // TODO: Handle EOF errors
    size_t start = span.start.index - span.start.column;
    input.clear();
    input.seekg(start);

    std::string code;
    if(!std::getline(input, code))
    {
        output << "(failed to read code)" << std::endl;
        return;
    }
    
    // TODO: Handle multiline
    output << " | " << code << std::endl << " | ";
    for(size_t s = 0; s < span.start.column; s++)
        output << " ";

    for(size_t s = 0; s < span.size; s++)
        output << "^";

    output << std::endl;
}

}
