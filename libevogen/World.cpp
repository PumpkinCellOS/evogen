#include <libevogen/World.h>

#include <libevogen/Task.h>

namespace evo
{

void Chunk::generate_tasks(World const& world, Generator& generator) const
{
    // TODO: Handle compression in x,y axis (the full of blocks chunk expands to 32*32=1024 /fills now)
    // TODO: Reset handle flag on finish
    for(unsigned y = 0; y < SIZE; y++)
    {
        for(unsigned x = 0; x < SIZE; x++)
        {
            size_t same_blocks = 0;
            uint16_t last_block_index = 0;
            int saved_z = -1;
            for(unsigned z = 0; z < SIZE; z++)
            {
                auto& block_descriptor = block_at({x, y, z});
                if(block_descriptor.flags.handled)
                    continue;
                block_descriptor.flags.handled = true;
                static auto save = [&]() {
                    if(z == SIZE - 1)
                        same_blocks++;
                    auto block = world.block_from_index(last_block_index);
                    if(block.has_value())
                    {
                        assert(same_blocks >= 1);
                        if(same_blocks == 1)
                            generator.add_task<PlaceBlockTask>(
                                block.value(),
                                Vector<int>{static_cast<int>(x), static_cast<int>(y), static_cast<int>(saved_z)});
                        else
                            generator.add_task<FillBlocksTask>(
                                block.value(),
                                Vector<int>{static_cast<int>(x), static_cast<int>(y), saved_z},
                                Vector<int>{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z - 1)});
                    }
                    same_blocks = 0;
                    last_block_index = 0;
                    saved_z = -1;
                };
                switch(block_descriptor.kind)
                {
                    case BlockDescriptor::Empty:
                    case BlockDescriptor::Height:
                    case BlockDescriptor::Marker:
                        save();
                        break;
                    case BlockDescriptor::Block:
                    {
                        if(z < SIZE - 1)
                        {
                            if(last_block_index == block_descriptor.arg)
                            {
                                same_blocks++;
                            }
                            else
                            {
                                if(last_block_index != 0)
                                {
                                    std::cerr << "dupe end save " << x << ", " << y << ", " << z << std::endl;
                                    save();
                                }
                                last_block_index = block_descriptor.arg;
                                saved_z = z;
                                same_blocks = 1;
                            }
                        }
                        else
                        {
                            std::cerr << "last z save " << x << ", " << y << ", " << z << std::endl;
                            save();
                        }
                    }
                }
            }
        }
    }
}

void World::set_block_at(Vector<int> const& position, Block const& block)
{
    auto index = index_of(block);
    if(index.has_value())
        set_block_descriptor_at(position, BlockDescriptor::create_block(index.value()));
    else
    {
        uint16_t new_index = generate_index(block);
        set_block_descriptor_at(position, BlockDescriptor::create_block(new_index));
    }
}

void World::fill_blocks_at(Vector<int> const& start, Vector<int> const& end, Block const& block)
{
    // TODO: Optimize it
    int minx = std::min(start.x, end.x), maxx = std::max(start.x, end.x);
    int miny = std::min(start.y, end.y), maxy = std::max(start.y, end.y);
    int minz = std::min(start.z, end.z), maxz = std::max(start.z, end.z);
    std::cerr << "fill_blocks_at " << start.to_string() << " / " << end.to_string() << " = " << block.to_command_format() << std::endl;
    for(int x = minx; x <= maxx; x++)
    {
        for(int y = miny; y <= maxy; y++)
        {
            for(int z = minz; z <= maxz; z++)
            {
                set_block_at({x, y, z}, block);
            }
        }
    }
}

void World::set_block_descriptor_at(Vector<int> const& position, BlockDescriptor block)
{
    //std::cerr << "set_block_descriptor_at " << position.to_string() << " = " << block.arg << std::endl;
    ensure_chunk_at(chunk_position_from_block(position)).block_at(chunk_offset_from_block(position)) = block;
}

BlockDescriptor World::ensure_block_descriptor_at(Vector<int> const& position)
{
    //std::cerr << "ensure_block_descriptor_at " << position.to_string() << std::endl;
    return ensure_chunk_at(chunk_position_from_block(position)).block_at(chunk_offset_from_block(position));
}

Chunk& World::ensure_chunk_at(Vector<int> const& chunk_position)
{
    auto it = m_chunks.find(chunk_position);
    if(it == m_chunks.end())
    {
        auto result = m_chunks.try_emplace(chunk_position);
        std::cerr << "ensure_chunk_at: creating at " << chunk_position.to_string() << " = " << result.second << std::endl;
        initialize_chunk(result.first->second);
        it = result.first;
    }
    return it->second;
}

void World::initialize_chunk(Chunk&) const
{
}

uint16_t World::generate_index(Block const& block)
{
    m_current_index++;
    m_index_to_block.try_emplace(m_current_index, block);
    m_block_to_index.try_emplace(block, m_current_index);
    return m_current_index;
}

std::optional<Block> World::block_from_index(uint16_t index) const
{
    auto it = m_index_to_block.find(index);
    return it == m_index_to_block.end() ? std::optional<Block>() : std::optional<Block>(it->second);
}

std::optional<uint16_t> World::index_of(Block const& block) const
{
    auto it = m_block_to_index.find(block);
    return it == m_block_to_index.end() ? std::optional<uint16_t>() : std::optional<uint16_t>(it->second);
}

Vector<int> World::chunk_position_from_block(Vector<int> const& position)
{
    auto [ax, ay, az] = position;
    int px = ax / Chunk::SIZE; if(ax < 0) px--;
    int py = ay / Chunk::SIZE; if(ay < 0) py--;
    int pz = az / Chunk::SIZE; if(az < 0) pz--;
    //std::cerr << "chunk_position_from_block " << px << "," << py << "," << pz << std::endl;
    return {px, py, pz};
}

Vector<unsigned> World::chunk_offset_from_block(Vector<int> const& position)
{
    auto [ax, ay, az] = position;
    // FIXME: Is there simpler way to do this?
    unsigned px = ax < 0 ? (Chunk::SIZE - (-ax) % Chunk::SIZE) % Chunk::SIZE : ax % Chunk::SIZE;
    unsigned py = ay < 0 ? (Chunk::SIZE - (-ay) % Chunk::SIZE) % Chunk::SIZE : ay % Chunk::SIZE;
    unsigned pz = az < 0 ? (Chunk::SIZE - (-az) % Chunk::SIZE) % Chunk::SIZE : az % Chunk::SIZE;
    return {px, py, pz};
}

Vector<int> World::block_from_chunk_position_and_offset(Vector<int> const& position, Vector<unsigned> const& offset)
{
    auto [px, py, pz] = position;
    auto [ox, oy, oz] = offset;
    int bx = Chunk::SIZE * px + ox;
    int by = Chunk::SIZE * py + oy;
    int bz = Chunk::SIZE * pz + oz;
    return {bx, by, bz};
}

void World::generate_tasks(Generator& generator) const
{
    std::cerr << "Block index:" << std::endl;
    for(auto& it: m_index_to_block)
    {
        std::cerr << " - " << it.first << ": " << it.second.to_command_format() << std::endl;
    }

    std::cerr << "Chunks:" << std::endl;
    Vector<int> last_turtle_position = generator.turtle().start_position();
    for(auto& it: m_chunks)
    {
        auto position = block_from_chunk_position_and_offset(it.first);
        std::cerr << " - " << it.first.to_string() << " (" << position.to_string() << ")" << std::endl;
        generator.add_task<MoveTurtleTask>(position - last_turtle_position, generator.turtle());
        last_turtle_position = position;
        it.second.generate_tasks(*this, generator);
    }
}

}
