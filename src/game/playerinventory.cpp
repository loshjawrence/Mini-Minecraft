#include "playerinventory.h"
#include "chunkmanager.h"

bool PlayerInventory::ValidateSlot(int slot)
{
    return slot >= 0 && slot < INVENTORY_SLOTS - 1;
}

PlayerInventory::PlayerInventory()
{
    this->items = new Item[INVENTORY_SLOTS];
}

bool PlayerInventory::AddItem(const Item &item)
{
    for(int i = 0; i < INVENTORY_SLOTS; i++)
    {
        if(items[i] == item) // Remember item equality
        {
            items[i].quantity++;
            return true;
        }
    }

    for(int i = 0; i < INVENTORY_SLOTS; i++)
    {
        if(items[i].type == Item::NONE)
        {
            items[i] = item;
            return true;
        }
    }

    return false;
}

void PlayerInventory::SetCraftedResult(Item item)
{
    items[INVENTORY_SLOTS - 1] = item;
}

Item PlayerInventory::GetItem(int slot)
{
    if(slot < 0 && slot >= INVENTORY_SLOTS - 1)
        return Item();

    return items[slot];
}

bool PlayerInventory::SwapItem(int slotA, int slotB)
{
    if(slotA != 45 && slotB != 45 && (!ValidateSlot(slotA) || !ValidateSlot(slotB)))
        return false;

    Item itemA = items[slotA];
    Item itemB = items[slotB];

    items[slotA] = itemB;
    items[slotB] = itemA;

    return true;
}

void PlayerInventory::Consume(int slot)
{
    if(!ValidateSlot(slot))
        return;

    items[slot].quantity = items[slot].quantity - 1 <= 0 ? 0 : items[slot].quantity - 1;

    if(items[slot].quantity == 0)
        items[slot] = Item();
}

// This is too ugly, but it does the trick... gotta go fast
std::string Item::GetName()
{
    if(type == BLOCK)
    {
        switch (id) {
        case GRASS:
            return "Grass";
        case DIRT:
            return "Dirt";
        case WOOD:
            return "Wood";
        case LEAF:
            return "Leaf";
        case STONE:
            return "Stone";
        case BEDROCK:
            return "Bedrock";
        case COAL:
            return "Coal";
        case IRON_ORE:
            return "Iron Ore";
        case LAVA:
            return "Lava";
        case WATER:
            return "Water";
        case LAVA_FLOW:
            return "Lava Flow";
        case WATER_FLOW:
            return "Water Flow";
        }
    }
    else if(type == TOOL)
    {
        switch (id) {
        case Tool::DIAMOND_PICKAXE:
            return "Diamond Pickaxe";
        }
    }

    return "None";
}
