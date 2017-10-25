#include "uiinventoryslot.h"
#include "../chunkmanager.h"

void UIInventorySlot::Awake()
{
    this->slot = -1;
    this->button = nullptr;
    this->player = nullptr;
    this->uiItem = nullptr;
    this->showBackground = true;
}

void UIInventorySlot::PhysicsUpdate()
{
    if(slot != -1 && uiItem != nullptr)
        uiItem->SetItem(this->player->GetInventory()->GetItem(slot));

    if(button != nullptr)
        button->GetGameObject()->SetEnabled(showBackground);
}

void UIInventorySlot::Initialize(int slot, Player * player, std::function<void(int)> clickCallback, std::function<void(int)> hoverCallback, bool crafting)
{
    this->player = player;
    this->slot = crafting ? (slot + 36) : slot;
    this->clickCallback = clickCallback;
    this->hoverCallback = hoverCallback;
    this->currentItem = this->player->GetInventory()->GetItem(slot);

    button = GameObject::Instantiate("Slot")->AddComponent<UIButton>();
    button->SetTexture("/cis460-minecraft/textures/ui/inventory_slot.tga");
    button->SetColor(glm::vec4(.9,.9,.9,1));
    button->SetHoverColor(glm::vec4(1.25f,1.25f,1.25f,1));
    button->GetTransform()->UISetSize(button->GetTransform()->UIGetSize() * 4.f);
    button->AddClickListener(std::bind(&UIInventorySlot::OnSlotClicked, this));
    button->AddHoverListener(std::bind(&UIInventorySlot::OnSlotHovered, this));

    gameObject->GetTransform()->AddChild(button->GetGameObject()->GetTransform());

    // Hardcoded stuff, as we dont have any visual editor
    int inventoryWidth = 9;

    GameObject * itemContainer = GameObject::Instantiate();
    this->uiItem = itemContainer->AddComponent<UIItem>();
    itemContainer->GetTransform()->SetLocalPosition(glm::vec3(35,35, 0));

    int x = slot % inventoryWidth;
    int y = slot / inventoryWidth;

    if(crafting)
    {
        x = slot % 3;
        y = slot / 3;
        y += 5;
        x += 1;

        if(slot == 9)
        {
            x = 5;
            y = 6;
        }
    }

    glm::vec2 offset = (!crafting  && slot < 9) ? glm::vec2(26, 25) : glm::vec2(26, 50);
    glm::vec2 buttonSize = button->GetTransform()->UIGetSize();
    glm::vec2 position = offset + glm::vec2(x, y) * buttonSize;

    GetTransform()->AddChild(itemContainer->GetTransform());
    GetTransform()->UISetLocalPosition(position);
}

void UIInventorySlot::OnSlotHovered()
{
    if(slot >= 0)
        this->hoverCallback(slot);
}

void UIInventorySlot::OnSlotClicked()
{
    // We may need to do other things here    
    if(slot >= 0)
        this->clickCallback(slot);
}

int UIInventorySlot::GetSlot()
{
    return slot;
}

void UIInventorySlot::EnableBackground(bool enable)
{
    showBackground = enable;
}

void UIItem::OnItemQuantityChanged()
{
    this->text->SetText(std::to_string(currentItem.quantity));
    this->text->GetGameObject()->SetEnabled(currentItem.quantity > 1);
}

void UIItem::Awake()
{
    ItemView::Awake();

    this->text = GameObject::Instantiate("")->AddComponent<UIText>();
    this->text->SetTextSize(23);
    this->text->GetTransform()->UISetLocalPosition(glm::vec2(7, -35));
    this->text->GetTransform()->UISetZValue(-.01f);
    GetTransform()->AddChild(text->GetTransform());
}

void UIItem::Update()
{
    itemViewTransform->SetLocalRotation(glm::vec3(glm::radians(-15.f), Engine::Time() * .5, 0));
}

void UIItem::UpdateItem()
{
    ItemView::UpdateItem();

    this->text->SetText(std::to_string(currentItem.quantity));
    this->text->GetGameObject()->SetEnabled(currentItem.quantity > 1);

    if(currentItem.type == Item::BLOCK)
    {
        itemViewTransform->SetLocalScale(glm::vec3(35));
        itemViewTransform->SetLocalPosition(glm::vec3(0,0,-100));
    }
    else
    {
        itemViewTransform->SetLocalScale(glm::vec3(55, 55, 5.f));
        itemViewTransform->SetLocalPosition(glm::vec3(0, 5, -100));
    }
}

MeshRenderer *UIItem::InstantiateRenderer(GameObject *container)
{
    return container->AddComponent<UIRenderer>();
}
