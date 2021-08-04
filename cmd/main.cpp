#include "libevogen/Block.h"
#include <libevogen/Generator.h>
#include <libevogen/Task.h>

int main()
{
    evo::Generator generator;
    generator.add_task<evo::PlaceBlockTask>(evo::Block("chest", "facing=south", R"({Items:[{id:diamond,Count:64}]})"),
                            evo::BlockPosition{{0, 0, 0}});
    generator.add_task<evo::FillBlocksTask>(evo::Block("grass_block"),
                            evo::BlockPosition{{1, 1, 1}},
                            evo::BlockPosition{{5, 5, 5}}
    );
    generator.add_task<evo::MoveTurtleTask>(evo::BlockPosition{{0, 5, 0}}, generator.turtle());
    generator.generate_to_stdout();
    return 0;
}

