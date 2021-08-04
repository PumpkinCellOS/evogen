#include <libevogen/Generator.h>

#include <fstream>

namespace evo
{

void Generator::generate(std::ostream& stream) const
{
    m_stream = &stream;
    m_turtle.generate_spawn_command(stream);
    for(auto& task: m_tasks)
    {
        task->generate_code(*this);
    }
    stream << "kill @e[tag=evogen]" << std::endl;
    m_stream = nullptr;
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
