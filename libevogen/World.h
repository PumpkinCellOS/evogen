#pragma once

#include <libevogen/Block.h>
#include <libevogen/Chunk.h>
#include <libevogen/Generator.h>
#include <libevogen/Vector.h>

#include <cassert>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace evo
{

class Structure;

class World
{
public:
    void set_block_at(Vector<int> const&, Block const&);
    void fill_blocks_at(Vector<int> const& start, Vector<int> const& end, Block const&);

    void fill_blocks_hollow(Vector<int> const& start, Vector<int> const& end, Block const& outline, Block const& fill = Block("air"));
    void fill_blocks_outline(Vector<int> const& start, Vector<int> const& end, Block const& outline);

    // Position specifies position of all-negative corner of structure.
    void place_structure(Structure const&, Vector<int> const& position);

    void set_block_descriptor_at(Vector<int> const&, BlockDescriptor);
    BlockDescriptor ensure_block_descriptor_at(Vector<int> const&);
    BlockDescriptor* get_block_descriptor_at(Vector<int> const&);
    BlockDescriptor const* get_block_descriptor_at(Vector<int> const&) const;

    Chunk& ensure_chunk_at(Vector<int> const& chunk_position);
    Chunk* get_chunk_at(Vector<int> const& chunk_position);
    Chunk const* get_chunk_at(Vector<int> const& chunk_position) const;

    // TODO: Handle y chunks
    static Vector<int> chunk_position_from_block(Vector<int> const&);
    static Vector<unsigned> chunk_offset_from_block(Vector<int> const&);
    static Vector<int> block_from_chunk_position_and_offset(Vector<int> const& position, Vector<unsigned> const& offset = {});

    void generate_tasks(Generator&) const;

    std::optional<Block> block_from_index(uint16_t) const;
    std::optional<uint16_t> index_of(Block const&) const;

protected:
    void initialize_chunk(Chunk&) const;
    uint16_t generate_index(Block const&);

private:
    std::unordered_map<Vector<int>, Chunk> m_chunks;
    std::unordered_map<uint16_t, Block> m_index_to_block;
    std::unordered_map<Block, uint16_t> m_block_to_index;
    uint16_t m_current_index = 0;
};

}
