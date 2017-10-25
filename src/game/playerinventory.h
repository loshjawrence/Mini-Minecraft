#ifndef PLAYERINVENTORY_H
#define PLAYERINVENTORY_H

#define INVENTORY_SLOTS 46 // 36 + 9 crafting + 1 result

#include <string>

struct Tool {
    enum Type
    {
        NONE = 0,
        DIAMOND_PICKAXE = 1,
        DIAMOND_SWORD = 2,
    };
};

struct Item
{
public:
    enum ItemType
    {
        NONE = 0,
        BLOCK = 1,
        TOOL = 2,
        OTHER = 3,
    };

    ItemType type;
    int id;
    int quantity;

    Item() : Item(NONE, 0, 0){}
    Item(ItemType type, int id, int quantity) : type(type), id(id), quantity(quantity) { }

    bool operator==(const Item& rhs)
    {
        return type == rhs.type && id == rhs.id; // Quantity is not considered for equality
    }

    std::string GetName();
};

class PlayerInventory // Not a component
{
protected:
    Item * items;

    bool ValidateSlot(int slot);

public:
    PlayerInventory();

    bool AddItem(const Item& item);
    void SetCraftedResult(Item item);

    Item GetItem(int slot);
    bool SwapItem(int slotA, int slotB);
    void Consume(int slot);
};

#endif // PLAYERINVENTORY_H
