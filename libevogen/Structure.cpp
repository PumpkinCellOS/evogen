#include <libevogen/Structure.h>

#include <cpp-nbt/nbt.hpp>
#include <fstream>

namespace evo
{

bool Structure::load_from_file(std::string const& name, Format format)
{
    std::ifstream file(name);
    if(!file.good())
        return false;

    try
    {
        nbt::NBT nbt(file);
        auto size = nbt::get_list<nbt::TagInt>(nbt.at<nbt::TagList>("size"));
        // FIXME: Are negative sizes allowed?
        m_size = {size[0], size[1], size[2]};
        std::cerr << "Loading palette" << std::endl;
        auto palette = nbt::get_list<nbt::TagCompound>(nbt.at<nbt::TagList>("palette"));

        // TODO: Handle this
        //auto palettes = nbt::get_list<nbt::TagCompound>(nbt.at<nbt::TagList>("palettes"));
        std::cerr << "Loading blocks" << std::endl;
        auto blocks = nbt::get_list<nbt::TagCompound>(nbt.at<nbt::TagList>("blocks"));
        for(auto& block : blocks)
        {
            auto state = block.at<nbt::TagInt>("state");
            auto pos = nbt::get_list<nbt::TagInt>(block.at<nbt::TagList>("pos"));
            // TODO: Handle block NBT
            auto position_vec = Vector<int>{pos[0], pos[1], pos[2]};
            auto palette_entry = palette[state];
            auto name = palette_entry.at<nbt::TagString>("Name");
            nbt::TagCompound properties;
            try
            {
                properties = palette_entry.at<nbt::TagCompound>("Properties");
            }
            catch(...) {}
            // TODO: Handle states
            BlockStates states;
            for(auto& property : properties.base)
            {
                auto property_string = std::get_if<nbt::TagString>(&property.second);
                if(!property_string)
                {
                    std::cerr << "Blockstate property must be a String" << std::endl;
                    return false;
                }
                states.set_state(property.first, *property_string);
            }

            set_block_at(position_vec, {name, states});
        }
        // TODO: Handle this
        //auto entities = nbt::get_list<nbt::TagCompound>(nbt.at<nbt::TagList>("entities"));
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception during loading NBT: " << e.what() << std::endl;
        return false;
    }
    std::cerr << "Structure loaded from file " << name << ": " << std::endl;
    std::cerr << "   size = " << m_size.to_string() << std::endl;
    return true;
}

}
