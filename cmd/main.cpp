#include "libevogen/Block.h"
#include <libevogen/Generator.h>
#include <libevogen/Task.h>

int main()
{
    evo::Generator generator;
    generator.add_task<evo::PlaceBlockTask>(evo::Block("chest", "facing=south", R"({Items:[{id:diamond,Count:64}]})"),
                            evo::BlockPosition::create_absolute({0, 0, 0}));
    generator.add_task<evo::FillBlocksTask>(evo::Block("grass_block"),
                            evo::BlockPosition::create_relative({1, 1, 1}),
                            evo::BlockPosition::create_relative({5, 5, 5})
    );
    generator.add_task<evo::MoveTurtleTask>(evo::BlockPosition::create_relative({0, 5, 0}), generator.turtle());
    generator.generate_to_stdout();
    return 0;
}

