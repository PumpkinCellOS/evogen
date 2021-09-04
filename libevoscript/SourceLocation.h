#pragma once

#include <cstddef>
#include <ostream>

namespace evo::script
{

struct SourceLocation
{
    size_t line = 0;
    size_t column = 0;
};

std::ostream& operator<<(std::ostream& stream, SourceLocation const& node);

}
