#include <libevogen/World.h>

namespace evo
{

void World::generate_tasks(Generator& generator) const
{
    std::cerr << "Block index: size: " << m_index_to_block.size() << std::endl;
    /*for(auto& it: m_index_to_block)
    {
        std::cerr << " - " << it.first << ": " << it.second.to_command_format() << std::endl;
    }*/

    std::cerr << "Block index: size: " << m_index_to_block.size() << std::endl;

    std::cerr << "Chunks: count = " << m_chunks.size() << std::endl;
    Vector<int> last_turtle_position = generator.turtle().start_position();
    for(auto& it: m_chunks)
    {
        auto position = block_from_chunk_position_and_offset(it.first);
        //std::cerr << " - " << it.first.to_string() << " (" << position.to_string() << ")" << std::endl;
        generator.add_task<MoveTurtleTask>(position - last_turtle_position, generator.turtle());
        last_turtle_position = position;
        it.second.generate_tasks(*this, generator);
    }
}

}
