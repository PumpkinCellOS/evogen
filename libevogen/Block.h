#pragma once

#include <libevogen/Vector.h>

#include <string>

namespace evo
{

class Block
{
public:
    Block()
    : m_id{}, m_states{}, m_nbt{} {}

    Block(std::string const& id, std::string const& states = "", std::string const& nbt = "")
    : m_id(id), m_states(states), m_nbt(nbt) {}

    std::string id() const { return m_id; }
    std::string states() const { return m_states; }
    std::string nbt() const { return m_nbt; }

    std::string to_command_format() const { return m_id + "[" + m_states + "]" + m_nbt; }

private:
    std::string m_id;
    std::string m_states;
    std::string m_nbt;
};

class BlockPosition
{
public:
    static BlockPosition create_absolute(Vector<int> const& position) { return BlockPosition(position, false); }
    static BlockPosition create_relative(Vector<int> const& position) { return BlockPosition(position, true); };

    bool is_relative() const { return m_relative; }

    std::string to_command_format() const
    {
        return  coord_to_string(m_position.x, m_relative) + " " +
                coord_to_string(m_position.y, m_relative) + " " +
                coord_to_string(m_position.z, m_relative);
    }

    Vector<int> resolve_relative_position(Vector<int> const& other) const
    { 
        return m_relative ? (other + m_position) : m_position;
    }

private:
    static std::string coord_to_string(int value, bool relative)
    {
        return (relative ? "~" : "") + std::to_string(value);
    }

    BlockPosition(Vector<int> const& position, bool relative)
    : m_position(position), m_relative(relative) {}

    Vector<int> m_position;
    bool m_relative = false;
};

}
