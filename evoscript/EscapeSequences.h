#pragma once

#include <string>

namespace evo::script::escapes
{

inline std::string csi(std::string const& value) { return "\e[" + value; }
inline std::string sgr(std::string const& value) { return "\e[" + value + "m"; }
inline std::string sgr_reset() { return "\e[m"; }

inline std::string constant(std::string const& value) { return "\e[36m" + value + "\e[0m"; }
inline std::string keyword(std::string const& value) { return "\e[94m" + value + "\e[0m"; }
inline std::string literal(std::string const& value) { return "\e[95m" + value + "\e[0m"; }
inline std::string error(std::string const& value) { return "\e[31m" + value + "\e[0m"; }
inline std::string type(std::string const& value) { return "\e[32m" + value + "\e[0m"; }
inline std::string name(std::string const& value) { return "\e[3m" + value + "\e[0m"; }
inline std::string comment(std::string const& value) { return "\e[90m" + value + "\e[0m"; }

}
