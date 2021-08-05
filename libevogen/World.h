#pragma once

#include <libevogen/Block.h>
#include <libevogen/Generator.h>
#include <libevogen/Vector.h>

#include <cassert>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace evo
{

struct BlockDescriptorFlags
{
    mutable uint8_t handled : 1 = 0;  // Already handled by generator
    uint8_t reserved : 7 = 0;
};

class BlockDescriptor
{
public:
    // Values not used here are Reserved.
    enum Kind : uint8_t
    {
        Empty,      // No block should be placed. `arg` is ignored.
        Block,      // A block with states index[`arg`] should be placed
        Height,     // All blocks below that block are currently set to marker_index[`arg`]. Created when loading heightmaps.
        Marker,     // Block is currently set to marker_index[`arg`]. Created when loading marker images.
    };

    Kind kind = Empty;
    BlockDescriptorFlags flags;
    uint16_t arg = 0;

    static BlockDescriptor create_empty() { return BlockDescriptor{.kind = Empty}; }
    static BlockDescriptor create_block(uint16_t index) { return BlockDescriptor{.kind = Block, .arg = index}; }
    static BlockDescriptor create_heightmap(uint16_t index) { return BlockDescriptor{.kind = Height, .arg = index}; }
    static BlockDescriptor create_marker(uint16_t index) { return BlockDescriptor{.kind = Marker, .arg = index}; }
};

class World;

// These chunks != Minecraft chunks!
class Chunk
{
public:
    static constexpr int SIZE = 32;

    Chunk() = default;
    explicit Chunk(Chunk const& other) = default;

    BlockDescriptor& block_at(Vector<unsigned> const& position)
    {
        assert(position.x < SIZE && position.y < SIZE && position.z < SIZE);
        return m_blocks[position.x][position.y][position.z];
    }

    BlockDescriptor const& block_at(Vector<unsigned> const&  position) const
    {
        assert(position.x < SIZE && position.y < SIZE && position.z < SIZE);
        return m_blocks[position.x][position.y][position.z];
    }

    void generate_tasks(World const& world, Generator&) const;

private:
    BlockDescriptor m_blocks[SIZE][SIZE][SIZE] = {};
};

class World
{
public:
    void set_block_at(Vector<int> const&, Block const&);
    void fill_blocks_at(Vector<int> const& start, Vector<int> const& end, Block const&);

    void set_block_descriptor_at(Vector<int> const&, BlockDescriptor);
    BlockDescriptor ensure_block_descriptor_at(Vector<int> const&);

    Chunk& ensure_chunk_at(Vector<int> const& chunk_position);

    // TODO: Handle y chunks
    static Vector<int> chunk_position_from_block(Vector<int> const&);
    static Vector<unsigned> chunk_offset_from_block(Vector<int> const&);
    static Vector<int> block_from_chunk_position_and_offset(Vector<int> const& position, Vector<unsigned> const& offset = {});

    void generate_tasks(Generator&) const;

    std::optional<Block> block_from_index(uint16_t) const;
    std::optional<uint16_t> index_of(Block const&) const;

private:
    void initialize_chunk(Chunk&) const;
    uint16_t generate_index(Block const&);

    std::unordered_map<Vector<int>, Chunk> m_chunks;
    std::unordered_map<uint16_t, Block> m_index_to_block;
    std::unordered_map<Block, uint16_t> m_block_to_index;
    uint16_t m_current_index = 0;
};

}
