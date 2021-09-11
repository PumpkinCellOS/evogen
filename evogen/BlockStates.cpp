#include <evogen/BlockStates.h>

#include <cassert>
#include <sstream>

namespace evo
{

BlockStates BlockStates::from_string(std::string const& input)
{
    // TODO
    assert(false);
}
   
std::string BlockStates::to_string() const
{
    std::ostringstream oss;
    size_t counter = 0;
    for(auto& state: m_states)
    {
        oss << state.first << "=" << state.second;
        if(counter != m_states.size() - 1)
            oss << ",";
        counter++;
    }
    return oss.str();
}

}
