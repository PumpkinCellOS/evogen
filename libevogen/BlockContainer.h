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

class BlockContainer
{
public:
    void set_block_at(Vector<int> const&, Block const&);
    void fill_blocks_at(Vector<int> const& start, Vector<int> const& end, Block const&);

    void fill_blocks_hollow(Vector<int> const& start, Vector<int> const& end, Block const& outline, Block const& fill = Block("air"));
    void fill_blocks_outline(Vector<int> const& start, Vector<int> const& end, Block const& outline);

    // Position specifies position of all-negative corner of structure.
    void place_structure(Structure const&, Vector<int> const& position);

    // Predicate: function of type std::optional<Block>(Vector<int> const& center_offset)
    template<class Predicate>
    void fill_blocks_if(Vector<int> const& start, Vector<int> const& end, Predicate&& predicate)
    {
        // TODO: Optimize it
        int minx = std::min(start.x, end.x), maxx = std::max(start.x, end.x);
        int miny = std::min(start.y, end.y), maxy = std::max(start.y, end.y);
        int minz = std::min(start.z, end.z), maxz = std::max(start.z, end.z);
        std::cerr << "fill_blocks_if " << start.to_string() << " / " << end.to_string() << std::endl;
        auto center = (Vector<int>{minx, miny, minz} + Vector<int>{maxx, maxy, maxz}) / 2.0;
        for(int x = minx; x <= maxx; x++)
        {
            for(int y = miny; y <= maxy; y++)
            {
                for(int z = minz; z <= maxz; z++)
                {
                    auto offset_vector = Vector<int>{x, y, z} - center;
                    auto block = predicate(offset_vector);
                    if(block.has_value())
                        set_block_at({x, y, z}, block.value());
                }
            }
        }
    }

    void fill_ball(Vector<int> const& center, double radius, Block const& block);
    void fill_cylinder(Vector<int> const& bottom_side_center, double radius, double height, Block const& block);

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

    std::optional<Block> block_from_index(uint16_t) const;
    std::optional<uint16_t> index_of(Block const&) const;

protected:
    void initialize_chunk(Chunk&) const;
    uint16_t generate_index(Block const&);

    std::unordered_map<uint16_t, Block> m_index_to_block;
    std::unordered_map<Vector<int>, Chunk> m_chunks;

private:
    std::unordered_map<Block, uint16_t> m_block_to_index;
    uint16_t m_current_index = 0;
};

}
