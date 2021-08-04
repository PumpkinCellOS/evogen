#include <libevogen/Turtle.h>

namespace evo
{

void Turtle::generate_spawn_command(std::ostream& stream) const
{
    stream << "summon armor_stand " << m_start_position.to_string() << R"( {Tags:["evogen"],NoGravity:1,Invisible:1,CustomName:"\")" + m_custom_name + R"(\""})" << std::endl;
}

}
