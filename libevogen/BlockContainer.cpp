#include <libevogen/World.h>

#include <libevogen/Structure.h>
#include <libevogen/Task.h>

namespace evo
{

void BlockContainer::set_block_at(Vector<int> const& position, Block const& block)
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

void BlockContainer::fill_blocks_at(Vector<int> const& start, Vector<int> const& end, Block const& block)
{
    // TODO: Optimize it
    int minx = std::min(start.x, end.x), maxx = std::max(start.x, end.x);
    int miny = std::min(start.y, end.y), maxy = std::max(start.y, end.y);
    int minz = std::min(start.z, end.z), maxz = std::max(start.z, end.z);
    //std::cerr << "fill_blocks_at " << start.to_string() << " / " << end.to_string() << " = " << block.to_command_format() << std::endl;
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

void BlockContainer::fill_blocks_hollow(Vector<int> const& start, Vector<int> const& end, Block const& outline, Block const& fill)
{
    auto min_vector = Vector<int>{std::min(start.x, end.x), std::min(start.y, end.y), std::min(start.z, end.z)};
    auto max_vector = Vector<int>{std::max(start.x, end.x), std::max(start.y, end.y), std::max(start.z, end.z)};

    fill_blocks_at(min_vector + Vector<int>(1, 1, 1), max_vector - Vector<int>(1, 1, 1), fill);
    fill_blocks_outline(min_vector, max_vector, outline);
}

void BlockContainer::fill_blocks_outline(Vector<int> const& start, Vector<int> const& end, Block const& outline)
{
    auto min_vector = Vector<int>{std::min(start.x, end.x), std::min(start.y, end.y), std::min(start.z, end.z)};
    auto max_vector = Vector<int>{std::max(start.x, end.x), std::max(start.y, end.y), std::max(start.z, end.z)};

    // X Sides
    fill_blocks_at(Vector<int>(min_vector.x, min_vector.y, min_vector.z), Vector<int>(min_vector.x, max_vector.y, max_vector.z), outline);
    fill_blocks_at(Vector<int>(max_vector.x, min_vector.y, min_vector.z), Vector<int>(max_vector.x, max_vector.y, max_vector.z), outline);

    // Y Sides
    fill_blocks_at(Vector<int>(min_vector.x + 1, min_vector.y, min_vector.z + 1), Vector<int>(max_vector.x - 1, min_vector.y, max_vector.z - 1), outline);
    fill_blocks_at(Vector<int>(min_vector.x + 1, max_vector.y, min_vector.z + 1), Vector<int>(max_vector.x - 1, max_vector.y, max_vector.z - 1), outline);

    // Z Sides
    fill_blocks_at(Vector<int>(min_vector.x + 1, min_vector.y, min_vector.z), Vector<int>(max_vector.x - 1, max_vector.y, min_vector.z), outline);
    fill_blocks_at(Vector<int>(min_vector.x + 1, min_vector.y, max_vector.z), Vector<int>(max_vector.x - 1, max_vector.y, max_vector.z), outline);
}

void BlockContainer::fill_ball(Vector<int> const& center, double radius, Block const& block)
{
    auto start = center - Vector<int>(radius+1, radius+1, radius+1);
    auto end = center + Vector<int>(radius+1, radius+1, radius+1);
    //std::cerr << "fill_ball r=" << radius << " : " << start.to_string() << "/" << end.to_string() << " = " << block.to_command_format() << std::endl;
    fill_blocks_if(start, end, [&](Vector<int> const& offset) {
        return offset.x * offset.x + offset.y * offset.y + offset.z * offset.z < radius * radius ?
            block : std::optional<Block>();
    });
}

void BlockContainer::fill_cylinder(Vector<int> const& bottom_side_center, double radius, double height, Block const& block)
{
    auto start = bottom_side_center + Vector<int>(-radius-1, 0, -radius-1);
    auto end = bottom_side_center + Vector<int>(radius+1, height, radius+1);
    //std::cerr << "fill_cylinder r=" << radius << " h=" << height << " : " << start.to_string() << "/" << end.to_string() << " = " << block.to_command_format() << std::endl;
    fill_blocks_if(start, end, [&](Vector<int> const& offset) {
        return offset.x * offset.x + offset.z * offset.z < radius * radius ?
            block : std::optional<Block>();
    });
}

void BlockContainer::place_structure(Structure const& structure, Vector<int> const& offset)
{
    auto [sx, sy, sz] = structure.size();
    for(int x = 0; x < sx; x++)
    {
        for(int y = 0; y < sy; y++)
        {
            for(int z = 0; z < sz; z++)
            {
                auto descriptor = structure.get_block_descriptor_at({x, y, z});
                assert(descriptor);
                if(descriptor->kind == BlockDescriptor::Block)
                {
                    auto block = structure.block_from_index(descriptor->arg);
                    assert(block.has_value());
                    // TODO: Handle nbt
                    set_block_at(Vector<int>(x, y, z) + offset, block.value());
                }
            }
        }
    }
}

void BlockContainer::set_block_descriptor_at(Vector<int> const& position, BlockDescriptor block)
{
    //std::cerr << "set_block_descriptor_at " << position.to_string() << " = " << block.arg << std::endl;
    ensure_chunk_at(chunk_position_from_block(position)).block_at(chunk_offset_from_block(position)) = block;
}

BlockDescriptor BlockContainer::ensure_block_descriptor_at(Vector<int> const& position)
{
    //std::cerr << "ensure_block_descriptor_at " << position.to_string() << std::endl;
    return ensure_chunk_at(chunk_position_from_block(position)).block_at(chunk_offset_from_block(position));
}

BlockDescriptor* BlockContainer::get_block_descriptor_at(Vector<int> const& position)
{
    auto chunk = get_chunk_at(chunk_position_from_block(position));
    if(!chunk)
        return nullptr;
    return &chunk->block_at(chunk_offset_from_block(position));
}

BlockDescriptor const* BlockContainer::get_block_descriptor_at(Vector<int> const& position) const
{
    auto chunk = get_chunk_at(chunk_position_from_block(position));
    if(!chunk)
        return nullptr;
    return &chunk->block_at(chunk_offset_from_block(position));
}

Chunk& BlockContainer::ensure_chunk_at(Vector<int> const& chunk_position)
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

Chunk* BlockContainer::get_chunk_at(Vector<int> const& chunk_position)
{
    auto it = m_chunks.find(chunk_position);
    if(it == m_chunks.end())
        return nullptr;
    return &it->second;
}

Chunk const* BlockContainer::get_chunk_at(Vector<int> const& chunk_position) const
{
    auto it = m_chunks.find(chunk_position);
    if(it == m_chunks.end())
        return nullptr;
    return &it->second;
}

void BlockContainer::initialize_chunk(Chunk&) const
{
}

uint16_t BlockContainer::generate_index(Block const& block)
{
    m_current_index++;
    m_index_to_block.try_emplace(m_current_index, block);
    m_block_to_index.try_emplace(block, m_current_index);
    return m_current_index;
}

std::optional<Block> BlockContainer::block_from_index(uint16_t index) const
{
    auto it = m_index_to_block.find(index);
    return it == m_index_to_block.end() ? std::optional<Block>() : std::optional<Block>(it->second);
}

std::optional<uint16_t> BlockContainer::index_of(Block const& block) const
{
    auto it = m_block_to_index.find(block);
    return it == m_block_to_index.end() ? std::optional<uint16_t>() : std::optional<uint16_t>(it->second);
}

Vector<int> BlockContainer::chunk_position_from_block(Vector<int> const& position)
{
    auto [ax, ay, az] = position;
    if(ax < -1) ax++;
    if(ay < -1) ay++;
    if(az < -1) az++;
    int px = ax / Chunk::SIZE; if(ax < 0) px--;
    int py = ay / Chunk::SIZE; if(ay < 0) py--;
    int pz = az / Chunk::SIZE; if(az < 0) pz--;
    //std::cerr << "chunk_position_from_block " << px << "," << py << "," << pz << std::endl;
    return {px, py, pz};
}

Vector<unsigned> BlockContainer::chunk_offset_from_block(Vector<int> const& position)
{
    auto [ax, ay, az] = position;
    // FIXME: Is there simpler way to do this?
    unsigned px = ax < 0 ? (Chunk::SIZE - (-ax) % Chunk::SIZE) % Chunk::SIZE : ax % Chunk::SIZE;
    unsigned py = ay < 0 ? (Chunk::SIZE - (-ay) % Chunk::SIZE) % Chunk::SIZE : ay % Chunk::SIZE;
    unsigned pz = az < 0 ? (Chunk::SIZE - (-az) % Chunk::SIZE) % Chunk::SIZE : az % Chunk::SIZE;
    return {px, py, pz};
}

Vector<int> BlockContainer::block_from_chunk_position_and_offset(Vector<int> const& position, Vector<unsigned> const& offset)
{
    auto [px, py, pz] = position;
    auto [ox, oy, oz] = offset;
    int bx = Chunk::SIZE * px + ox;
    int by = Chunk::SIZE * py + oy;
    int bz = Chunk::SIZE * pz + oz;
    return {bx, by, bz};
}

}
