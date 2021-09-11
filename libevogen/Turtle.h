#pragma once

#include <libevogen/Vector.h>

#include <ostream>
#include <string>

namespace evo
{

class Turtle
{
public:
    Turtle(Vector<int> start_position, std::string custom_name = "evoTurtle")
    : m_start_position(start_position), m_current_position(start_position), m_custom_name(custom_name) {}

    Vector<int> start_position() const { return m_start_position; }

    void generate_spawn_command(std::ostream&) const;

    std::string to_strict_selector() const { return "@e[tag=evogen,type=armor_stand,name=" + m_custom_name + "]"; }
    std::string to_general_selector() const { return "@e[tag=evogen]"; }

    // Use for teleporting (e.g player)
    std::string to_execute_as() const { return "execute as " + to_general_selector(); }

    // Use for filling, when you need to execute command once
    std::string to_execute_at() const { return "execute at " + to_strict_selector(); }

    void move(Vector<int> const& vector) { m_current_position += vector; }
    Vector<int> position() const { return m_current_position; }

private:
    Vector<int> m_start_position;
    Vector<int> m_current_position;
    std::string m_custom_name;
};

}
