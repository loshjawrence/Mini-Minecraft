#ifndef INVENTORYTOOLBOX_H
#define INVENTORYTOOLBOX_H

#include "../../engine/common.h"
#include <vector>

class UIInventorySlot;
class Player;

class UIInventoryToolbox : public UIComponent
{
protected:
    UIImage * backgroundImage;
    UIImage * selectedElement;
    Player * player;
    std::vector<UIInventorySlot*> quickSlots;

    int currentSlot;

    void AddSlot(int slot);
    void OnSlotClicked(int slot);

public:
    ~UIInventoryToolbox();

    void Awake();
    void Start();
    void Update();

    void SetPlayer(Player *player);
    void SelectSlot(int slot);
};

#endif // INVENTORYTOOLBOX_H
