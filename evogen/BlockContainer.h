#pragma once

#include <evogen/Block.h>
#include <evogen/Chunk.h>
#include <evogen/Generator.h>
#include <evogen/Image.h>
#include <evogen/Vector.h>

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

    // Image coords (pixels) correspond to world coords + offset (Y = 0).
    // y - Y coordinate to place markers on
    // World coords = {Image.x + offset.x, offset.y + y, Image.y + offset.z}
    // Marker is placed if alpha >= 128.
    // The color is 5-bit (is shifted right by 3) when creating marker value.
    // TODO: Avoid that rounding!
    void load_markers_from_image(Image const&, int y = 0, Vector<int> const& offset = {});

    // Predicate: function of type std::optional<Block>(Vector<int> const& center_offset)
    template<class Predicate>
    void fill_blocks_if(Vector<int> const& start, Vector<int> const& end, Predicate&& predicate)
    {
        // TODO: Optimize it
        int minx = std::min(start.x, end.x), maxx = std::max(start.x, end.x);
        int miny = std::min(start.y, end.y), maxy = std::max(start.y, end.y);
        int minz = std::min(start.z, end.z), maxz = std::max(start.z, end.z);
        //std::cerr << "fill_blocks_if " << start.to_string() << " / " << end.to_string() << std::endl;
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

    void set_marker(uint16_t index, Block const&);
    void set_marker(Color const& color, Block const& block) { set_marker(marker_index_from_color(color), block); }

    std::optional<Block> block_from_marker_index(uint16_t index) const;
    std::optional<Block> block_from_marker_color(Color const& color) { return block_from_marker_index(marker_index_from_color(color)); }

    static uint16_t marker_index_from_color(Color const& color);

protected:
    void initialize_chunk(Chunk&) const;
    uint16_t generate_index(Block const&);

    std::unordered_map<uint16_t, Block> m_index_to_block;
    std::unordered_map<Vector<int>, Chunk> m_chunks;

    std::unordered_map<uint16_t, Block> m_marker_index_to_block;

private:
    std::unordered_map<Block, uint16_t> m_block_to_index;
    uint16_t m_current_index = 0;
};

}
