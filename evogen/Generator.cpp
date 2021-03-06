#include <evogen/Generator.h>

#include <evogen/World.h>

#include <fstream>

namespace evo
{

void Generator::load_from_world(World const& world)
{
    world.generate_tasks(*this);
}

void Generator::generate(std::ostream& stream) const
{
    m_stream = &stream;
    m_turtle.generate_spawn_command(stream);
    for(auto& task: m_tasks)
    {
        task->generate_code(*this);
    }
    stream << "kill " << m_turtle.to_strict_selector() << std::endl;
    m_stream = nullptr;
    std::cout << "Generated commands from " << m_tasks.size() << " tasks!" << std::endl;
}

bool Generator::generate_to_file(std::string const& name) const
{
    std::ofstream file(name);
    if(file.fail())
        return false;

    generate(file);
    return true;
}

}
