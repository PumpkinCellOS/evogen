#pragma once

#include <libevogen/BlockContainer.h>

namespace evo
{

class Structure : public BlockContainer
{
public:
    enum class Format
    {
        StructureBlock
        // TODO: Schematic
        // TODO: Extract from world
    };

    bool load_from_file(std::string const& name, Format format);

    Vector<int> size() const { return m_size; }

private:
    Vector<int> m_size;
};

}

