#include <evogen/Block.h>

namespace evo
{

static constexpr char const* BLOCK_IDS[] {
    "air",
    "stone",
    "granite",
    "polished_granite",
    "diorite",
    "polished_diorite",
    "andesite",
    "polished_andesite",
    "grass_block",
    "dirt",
    "coarse_dirt",
    "podzol",
    "cobblestone",
    "oak_planks",
    "spruce_planks",
    "birch_planks",
    "jungle_planks",
    "acacia_planks",
    "dark_oak_planks",
    "oak_sapling",
    "spruce_sapling",
    "birch_sapling",
    "jungle_sapling",
    "acacia_sapling",
    "dark_oak_sapling",
    "bedrock",
    "sand",
    "red_sand",
    "gravel",
    "gold_ore",
    "iron_ore",
    "coal_ore",
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "dark_oak_log",
};

Block::Block(VanillaBlock type, BlockStates const& states, std::string const& nbt)
: Block(BLOCK_IDS[static_cast<size_t>(type)], states, nbt) {}

}
