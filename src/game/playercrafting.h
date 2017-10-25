#ifndef PLAYERCRAFTING_H
#define PLAYERCRAFTING_H

#include "playerinventory.h"
#include <vector>

class PlayerCrafting
{
protected:
    std::vector<std::vector<Item>> sources;
    std::vector<Item> results;

    void DefineRecipes();

public:
    PlayerCrafting();

    Item Craft(std::vector<Item> input);
    void AddRecipe(std::vector<Item> source, Item result);
};

#endif // PLAYERCRAFTING_H
