#include <libevogen/Block.h>
#include <libevogen/Generator.h>
#include <libevogen/Structure.h>
#include <libevogen/Task.h>
#include <libevogen/VanillaBlock.h>
#include <libevogen/World.h>

int main()
{
    evo::World world;
    
    world.set_block_at({10, 10, 10}, evo::VanillaBlock::Stone);
    world.fill_blocks_at({11, 11, 11}, {-11, 50, -11}, evo::VanillaBlock::Stone);
    world.fill_blocks_outline({-50, 50, -50}, {-40, 40, -40}, evo::VanillaBlock::OakPlanks);
    world.fill_blocks_hollow({50, 50, 50}, {40, 40, 40}, evo::VanillaBlock::OakLog, evo::VanillaBlock::Podzol);
    world.fill_blocks_if({50, 100, 50}, {40, 140, 40}, [](auto& offset)->std::optional<evo::Block> {
        if(offset.x % 2 == offset.y % 2)
            return {evo::VanillaBlock::OakLog};
        return {evo::VanillaBlock::Stone};
    });

    evo::Structure structure;
    if(!structure.load_from_file("test.nbt", evo::Structure::Format::StructureBlock))
    {
        std::cerr << "error :(" << std::endl;
        return 1;
    }
    for(size_t x = 0; x < 10; x++)
    {
        for(size_t z = 0; z < 10; z++)
        {
            world.place_structure(structure, {static_cast<int>(x * (structure.size().x + 2)), 100, static_cast<int>(z * (structure.size().z + 2))});
        }
    }

    evo::Generator generator;
    generator.load_from_world(world);
    generator.generate_to_file("functions/output.mcfunction");
    return 0;
}

