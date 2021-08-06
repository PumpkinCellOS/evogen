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
    world.fill_blocks_hollow({50, 50, 50}, {40, 40, 40}, {evo::VanillaBlock::OakLog, "axis=x"}, evo::VanillaBlock::Podzol);

    evo::Structure structure;
    if(!structure.load_from_file("test.nbt", evo::Structure::Format::StructureBlock))
    {
        std::cerr << "error :(" << std::endl;
        return 1;
    }
    world.place_structure(structure, {0, 100, 0});

    evo::Generator generator;
    generator.load_from_world(world);
    generator.generate_to_file("functions/output.mcfunction");
    return 0;
}

