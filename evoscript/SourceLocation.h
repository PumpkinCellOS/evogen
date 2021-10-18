#pragma once

#include <cstddef>
#include <ostream>

namespace evo::script
{

struct SourceLocation
{
    size_t line = 0;
    size_t column = 0;
    size_t index = 0;
};

struct SourceSpan
{
    SourceLocation start;
    size_t size = 0;
};

std::ostream& operator<<(std::ostream& stream, SourceLocation const& node);

void display_source_range(std::ostream& output, std::istream& input, SourceSpan const& span);

}
