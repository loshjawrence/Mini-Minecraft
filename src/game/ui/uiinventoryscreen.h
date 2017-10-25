#ifndef UIINVENTORYSCREEN_H
#define UIINVENTORYSCREEN_H

#include "../playercrafting.h"
#include "../../engine/common.h"
#include "uiinventoryslot.h"
#include "../player.h"
#include <vector>

class UIInventoryScreen: public UIComponent, public InputListener
{
protected:
    UIImage * bgOverlay;
    UIImage * background;
    Player * player;
    PlayerCrafting crafting;

    UIItem * dragItem;
    UIText * itemDesc;
    bool hovered;

    UIInventorySlot * currentClickedSlot;
    std::vector<UIInventorySlot*> inventorySlots;

    ~UIInventoryScreen();

    void AddSlot(int slot);
    void AddCraftingSlot(int slotNumber);

    void OnSlotClicked(int slot);
    void OnSlotHovered(int slot);

    void CheckCrafting();

public:

    void Awake();
    void Start();

    void Update();

    void SetPlayer(Player * player);
    void OnKeyReleaseEvent(QKeyEvent *e);
};

#endif // UIINVENTORYSCREEN_H
