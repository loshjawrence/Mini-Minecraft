#ifndef UIINVENTORYSLOT_H
#define UIINVENTORYSLOT_H

#include "../../engine/common.h"
#include "../player.h"
#include "../itemview.h"
#include <functional>

class UIItem : public ItemView
{
protected:
    UIText * text;
    void OnItemQuantityChanged();

public:
    void Awake();
    void Update();
    void UpdateItem();
    MeshRenderer *InstantiateRenderer(GameObject *container);
};

class UIInventorySlot : public UIComponent
{
protected:
    Player * player;
    UIButton * button;
    UIItem * uiItem;
    std::function<void(int)> clickCallback;
    std::function<void(int)> hoverCallback;

    int slot;
    Item currentItem;
    bool showBackground;

public:

    void Awake();
    void PhysicsUpdate();

    void Initialize(int slot, Player *player, std::function<void(int)> clickCallback, std::function<void(int)> hoverCallback, bool crafting);

    void OnSlotHovered();
    void OnSlotClicked();
    int GetSlot();

    void EnableBackground(bool enable);
};

#endif // UIINVENTORYSLOT_H
