#pragma once

#include <libevogen/BlockStates.h>
#include <libevogen/VanillaBlock.h>
#include <libevogen/Vector.h>

#include <string>

namespace evo
{

class Block
{
public:
    Block()
    : m_id{}, m_states{}, m_nbt{} {}

    Block(std::string const& id, BlockStates const& states = {}, std::string const& nbt = "")
    : m_id(id), m_states(states), m_nbt(nbt) {}

    Block(VanillaBlock type, BlockStates const& states = {}, std::string const& nbt = "");

    std::string id() const { return m_id; }
    BlockStates states() const { return m_states; }
    std::string nbt() const { return m_nbt; }

    std::string to_command_format() const { return m_id + "[" + m_states.to_string() + "]" + m_nbt; }

    bool operator==(Block const& other) const
    {
        return id() == other.id() && states() == other.states() && nbt() == other.nbt();
    }

private:
    std::string m_id;
    BlockStates m_states;
    std::string m_nbt;
};

class BlockPosition
{
public:
    BlockPosition(Vector<int> const& position)
    : m_position(position) {}

    std::string to_command_format() const
    {
        return  coord_to_string(m_position.x) + " " +
                coord_to_string(m_position.y) + " " +
                coord_to_string(m_position.z);
    }

    Vector<int> resolve_relative_position(Vector<int> const& relative) const
    { 
        return relative + m_position;
    }

    Vector<int> resolve_absolute_position(Vector<int> const& absolute) const
    { 
        return absolute - m_position;
    }

    Vector<int> value() const { return m_position; }

private:
    static std::string coord_to_string(int value)
    {
        return "~" + std::to_string(value);
    }

    Vector<int> m_position;
};

}

namespace std
{

template<>
struct hash<evo::Block>
{
    size_t operator()(evo::Block const& block) const
    {
        return std::hash<std::string>()(block.to_command_format());
    }
};

}
