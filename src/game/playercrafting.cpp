#include "playercrafting.h"
#include "chunkmanager.h"

void PlayerCrafting::DefineRecipes()
{
    std::vector<Item> sources;

    // Just a pickaxe because time is running out!
//    sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item());
//    sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item());
//    sources.push_back(Item()); sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item());

    sources.push_back(Item()); sources.push_back(Item(Item::BLOCK, STONE, 1)); sources.push_back(Item());
    sources.push_back(Item()); sources.push_back(Item(Item::BLOCK, DIRT, 1)); sources.push_back(Item());
    sources.push_back(Item()); sources.push_back(Item(Item::BLOCK, WOOD, 1)); sources.push_back(Item());

    AddRecipe(sources, Item(Item::TOOL, Tool::DIAMOND_SWORD, 1));
}

PlayerCrafting::PlayerCrafting()
{
    DefineRecipes();
}

// Includes empty items!
Item PlayerCrafting::Craft(std::vector<Item> input)
{
    for(unsigned int i = 0; i < sources.size(); i++)
    {
        std::vector<Item>& source = sources[i];

        bool equal = true;

        for(unsigned int j = 0; j < source.size(); j++)
            if(!(source[j] == input[j]))
                equal = false;

        if(equal)
            return results[i];
    }

    return Item();
}

void PlayerCrafting::AddRecipe(std::vector<Item> source, Item result)
{
    this->sources.push_back(source);
    this->results.push_back(result);
}
