#include "Task.h"

#include <libevogen/Generator.h>

namespace evo
{

void PlaceBlockTask::generate_code(Generator const& generator) const
{
    *generator.stream() << generator.turtle().to_execute_at() << " run setblock "
                            << m_position.to_command_format() << " "
                            << m_block.to_command_format() << std::endl;
}

void FillBlocksTask::generate_code(Generator const& generator) const
{
    *generator.stream() << generator.turtle().to_execute_at() << " run fill "
                        << m_start_position.to_command_format() << " "
                        << m_end_position.to_command_format() << " "
                        << m_block.to_command_format() << std::endl;
}

void MoveTurtleTask::generate_code(Generator const& generator) const
{
    *generator.stream() << m_turtle.to_execute_as() << " at @s run tp @s "
                        << m_position.to_command_format() << std::endl;
    // DEBUG
    *generator.stream() << m_turtle.to_execute_as() << " at @s run setblock ~ ~ ~ redstone_block" << std::endl;
}

}
