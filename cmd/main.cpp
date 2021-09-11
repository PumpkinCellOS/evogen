#include <evogen/Block.h>
#include <evogen/Generator.h>
#include <evogen/Image.h>
#include <evogen/Structure.h>
#include <evogen/Task.h>
#include <evogen/VanillaBlock.h>
#include <evogen/World.h>

int main()
{
    evo::Image image;
    if(!image.load_from_file("image.png"))
        std::cout << "could not load image :(" << std::endl;

    evo::World world;

    world.load_markers_from_image(image, 63, {128, 0, 128});
    world.set_marker(evo::World::marker_index_from_color({255, 255, 0}), {"minecraft:yellow_wool"});
    world.set_marker(evo::World::marker_index_from_color({255, 0, 0}), {"minecraft:red_wool"});
    world.set_block_at({10, 10, 10}, evo::VanillaBlock::Stone);
    world.fill_blocks_at({11, 11, 11}, {-11, 50, -11}, evo::VanillaBlock::Stone);
    world.fill_blocks_outline({-50, 50, -50}, {-40, 40, -40}, evo::VanillaBlock::OakPlanks);
    world.fill_blocks_hollow({50, 50, 50}, {40, 40, 40}, evo::VanillaBlock::OakLog, evo::VanillaBlock::Podzol);
    world.fill_blocks_if({50, 100, 50}, {40, 140, 40}, [](auto& offset)->std::optional<evo::Block> {
        if(abs(offset.x % 2) == abs(offset.y % 2))
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

    world.fill_ball({-25, 20, -25}, 6, evo::VanillaBlock::Cobblestone);
    world.fill_cylinder({-25, 40, -25}, 10, 10, evo::VanillaBlock::AcaciaPlanks);

    evo::Generator generator;
    generator.load_from_world(world);
    generator.generate_to_file("functions/output.mcfunction");
    return 0;
}

