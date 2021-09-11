#include <evogen/Turtle.h>

namespace evo
{

void Turtle::generate_spawn_command(std::ostream& stream) const
{
    // Teleport player(s) that load chunks to start position
    stream << "tp @e[tag=evogen] " << m_start_position.to_string() << std::endl;

    // Actually summon the armor stand
    stream << "summon armor_stand " << m_start_position.to_string() << R"( {Tags:["evogen"],NoGravity:1,Invisible:1,CustomName:"\")" + m_custom_name + R"(\""})" << std::endl;
}

}
