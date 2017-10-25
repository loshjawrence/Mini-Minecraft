#include "player.h"
#include "playercross.h"
#include "playerinput.h"
#include "playercubehighlight.h"
#include "playerphysics.h"
#include "itemview.h"
#include "playerselecteditem.h"
#include "ui/inventorytoolbox.h"

void Player::Awake()
{
    // Base initialization of the player...
    this->gameObject->AddComponent<PlayerInput>();
    this->physics = this->gameObject->AddComponent<PlayerPhysics>();
    this->inventory = new PlayerInventory();
    this->selectedItemSlot = 0;
    this->toolbox = nullptr;

    GameObject * cameraContainer = GameObject::Instantiate(std::string("PlayerCamera"));
    this->camera = cameraContainer->AddComponent<PerspectiveCamera>();
    cameraContainer->GetTransform()->SetLocalPosition(glm::vec3(0,1.5,0));
    cameraContainer->GetTransform()->SetParent(gameObject->GetTransform());

    GameObject * cross = GameObject::Instantiate(std::string("Cross"));
    cross->AddComponent<PlayerCross>();
    cross->GetTransform()->SetLocalPosition(glm::vec3(0,0,1.5));
    cross->GetTransform()->SetLocalScale(glm::vec3(.05,.05,.05));
    cross->GetTransform()->SetParent(cameraContainer->GetTransform());

    GameObject * cubeHighlight = GameObject::Instantiate(std::string("CubeHighlight"));
    this->cubeHighlight = cubeHighlight->AddComponent<PlayerCubeHighlight>();
    cubeHighlight->GetTransform()->SetLocalPosition(glm::vec3(2,0,2));
    cubeHighlight->SetEnabled(false);

    inventory->AddItem(Item(Item::TOOL, Tool::DIAMOND_PICKAXE, 1));
    inventory->AddItem(Item(Item::BLOCK, GRASS, 10));
    inventory->AddItem(Item(Item::BLOCK, DIRT, 10));
    inventory->AddItem(Item(Item::BLOCK, WOOD, 10));
    inventory->AddItem(Item(Item::BLOCK, STONE, 10));

    this->selectedItem = GameObject::Instantiate("PlayerItem")->AddComponent<PlayerSelectedItem>();
    cameraContainer->GetTransform()->AddChild(selectedItem->GetTransform());
    SelectItemSlot(0);
}

void Player::SetToolbox(UIInventoryToolbox *toolbox)
{
    this->toolbox = toolbox;
}

PlayerInventory *Player::GetInventory()
{
    return inventory;
}

void Player::Move(const glm::vec3 translation, bool moveVertically)
{
    // The movement is based on the camera
    glm::vec3 forward = camera->GetGameObject()->GetTransform()->Forward();
    glm::vec3 right = camera->GetGameObject()->GetTransform()->Right();
    glm::vec3 up = camera->GetGameObject()->GetTransform()->Up();
    glm::vec3 displacement = right * translation.x + forward * translation.z + up * translation.y;

    if(moveVertically)
    {
        this->physics->MoveTowards(this->gameObject->GetTransform()->LocalPosition() + displacement);
    }
    else
    {
        float originalMagnitude = glm::length(displacement);
        displacement.y = 0.f; // Player does not move in vertical axis, but keep magnitude!

        if(glm::length(displacement) >= .0001f)
            displacement = glm::normalize(displacement) * originalMagnitude;

        glm::vec3 newPosition = this->gameObject->GetTransform()->LocalPosition() + displacement ;
        this->physics->MoveTowards(newPosition);
    }
}

void Player::Jump()
{
    this->physics->SetVelocity(glm::vec3(0,10,0));
}

PlayerSelectedItem *Player::GetSelectedItemView()
{
    return selectedItem;
}

Item Player::GetSelectedItem()
{
    return inventory->GetItem(selectedItemSlot);
}

void Player::SelectItemSlot(int slot)
{
    if(slot >= 0 && slot < 9)
    {
        Item item = inventory->GetItem(slot);
        this->selectedItemSlot = slot;
        this->selectedItem->SelectItem(item);

        if(this->toolbox != nullptr)
            this->toolbox->SelectSlot(slot);
    }
}

int Player::GetSelectedSlot()
{
    return selectedItemSlot;
}
