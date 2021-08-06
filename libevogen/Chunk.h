#pragma once

#include <libevogen/Vector.h>

#include <cassert>

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

class Generator;
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

}
