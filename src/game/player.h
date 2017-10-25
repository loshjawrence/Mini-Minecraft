#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/common.h"
#include "playerinventory.h"

class ItemView;
class PlayerPhysics;
class UIInventoryToolbox;
class PlayerSelectedItem;
class PlayerCubeHighlight;

class Player : public Component
{
private:
    // Not used for now
    int health;
    int hunger;

    PlayerInventory * inventory;
    PlayerSelectedItem * selectedItem;
    UIInventoryToolbox * toolbox;
    int selectedItemSlot; // 0-9

public:
    virtual void Awake();

    PlayerCubeHighlight * cubeHighlight;
    PlayerPhysics * physics;
    Camera * camera;

    // MVC? What is that?
    void SetToolbox(UIInventoryToolbox * toolbox);

    PlayerInventory * GetInventory();
    void Move(const glm::vec3 translation, bool moveVertically);
    void Jump();

    PlayerSelectedItem * GetSelectedItemView();
    Item GetSelectedItem();
    void SelectItemSlot(int slot);
    int GetSelectedSlot();
};

#endif // PLAYER_H
