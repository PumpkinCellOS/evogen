#pragma once

#include <map>
#include <string>

namespace evo
{

class BlockStates
{
public:
    BlockStates() = default;

    static BlockStates from_string(std::string const& input);

    std::string state(std::string const& name) const { auto it = m_states.find(name); if(it == m_states.end()) return ""; return it->second; }
    void set_state(std::string const& name, std::string const& value) { m_states[name] = value; }

    std::string to_string() const;

    bool operator==(BlockStates const& other) { return other.m_states == m_states; }

private:
    std::map<std::string, std::string> m_states;
};

}
