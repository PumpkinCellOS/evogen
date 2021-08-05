#include <libevogen/Block.h>
#include <libevogen/Generator.h>
#include <libevogen/Task.h>
#include <libevogen/World.h>

int main()
{
    evo::World world;
    
    world.set_block_at({10, 10, 10}, evo::Block("stone"));
    world.fill_blocks_at({11, 11, 11}, {-11, 50, -11}, evo::Block("stone"));

    evo::Generator generator;
    generator.load_from_world(world);
    generator.generate_to_file("functions/output.mcfunction");
    return 0;
}

