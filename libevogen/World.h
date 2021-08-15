#pragma once

#include <libevogen/BlockContainer.h>

namespace evo
{

class World : public BlockContainer
{
public:
    void generate_tasks(Generator&) const;
};

}
