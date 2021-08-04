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
    : m_start_position(start_position), m_custom_name(custom_name) {}

    Vector<int> start_position() const { return m_start_position; }

    void generate_spawn_command(std::ostream&) const;

    std::string to_selector() const { return "@e[tag=evogen,type=armor_stand,name=" + m_custom_name + "]"; }
    std::string to_execute_as() const { return "execute as " + to_selector(); }
    std::string to_execute_at() const { return "execute at " + to_selector(); }

private:
    Vector<int> m_start_position;
    std::string m_custom_name;
};

}
