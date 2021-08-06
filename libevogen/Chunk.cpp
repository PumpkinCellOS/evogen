#include <libevogen/Chunk.h>

#include <libevogen/Generator.h>
#include <libevogen/World.h>

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
                                    save();
                                last_block_index = block_descriptor.arg;
                                saved_z = z;
                                same_blocks = 1;
                            }
                        }
                        else
                            save();
                    }
                }
            }
        }
    }
}

}
