#include "uiinventoryscreen.h"
#include "../playerinput.h"

UIInventoryScreen::~UIInventoryScreen()
{
}

void UIInventoryScreen::AddSlot(int slotNumber)
{
    // Parent each slot to the inventory bg
    UIInventorySlot * slot = GameObject::Instantiate("")->AddComponent<UIInventorySlot>();
    gameObject->GetTransform()->AddChild(slot->GetGameObject()->GetTransform());
    slot->Initialize(slotNumber, player,
                     std::bind(&UIInventoryScreen::OnSlotClicked, this, std::placeholders::_1),
                     std::bind(&UIInventoryScreen::OnSlotHovered, this, std::placeholders::_1), false);
    inventorySlots.push_back(slot);
}

void UIInventoryScreen::AddCraftingSlot(int slotNumber)
{
    // Parent each slot to the inventory bg
    UIInventorySlot * slot = GameObject::Instantiate("")->AddComponent<UIInventorySlot>();
    gameObject->GetTransform()->AddChild(slot->GetGameObject()->GetTransform());

    slot->Initialize(slotNumber, player,
                     std::bind(&UIInventoryScreen::OnSlotClicked, this, std::placeholders::_1),
                     std::bind(&UIInventoryScreen::OnSlotHovered, this, std::placeholders::_1), true);

    inventorySlots.push_back(slot);
}

void UIInventoryScreen::OnSlotClicked(int slot)
{
    if(player == nullptr)
        return;

    if(this->currentClickedSlot != nullptr)
    {
        int srcSlot = currentClickedSlot->GetSlot();
        this->player->GetInventory()->SwapItem(srcSlot, slot);

        this->currentClickedSlot = nullptr;
        this->dragItem->SetItem(Item());
    }
    else
    {
        // Empty items cant be source
        if(player->GetInventory()->GetItem(slot).type != Item::NONE)
        {
            this->currentClickedSlot = inventorySlots[slot];
            this->dragItem->SetItem(player->GetInventory()->GetItem(slot));
        }
    }
}

void UIInventoryScreen::OnSlotHovered(int slot)
{
    glm::vec2 cursorPosition = Engine::GetCurrentMousePosition();
    cursorPosition.y = Engine::GetScreenSize().y - cursorPosition.y;

    Item item = player->GetInventory()->GetItem(slot);

    if(item.type != Item::NONE)
    {
        this->itemDesc->SetText(item.GetName());
        this->itemDesc->GetTransform()->UISetLocalPosition(cursorPosition - GetTransform()->UIGetLocalPosition());
        itemDesc->GetGameObject()->SetEnabled(true);
        this->hovered = true;
    }
}

void UIInventoryScreen::CheckCrafting()
{
    std::vector<Item> sources;

    for(int i = 0; i < 9; i++)
        sources.push_back(player->GetInventory()->GetItem(36 + i));

    Item result = crafting.Craft(sources);

    if(result.type != Item::NONE)
        player->GetInventory()->SetCraftedResult(result);
}

void UIInventoryScreen::Awake()
{
    this->bgOverlay = nullptr;
    this->player = nullptr;
    this->background = nullptr;
    this->currentClickedSlot = nullptr;
    this->dragItem = nullptr;
    this->itemDesc = nullptr;
    this->hovered = false;
}

void UIInventoryScreen::Start()
{
    bgOverlay = GameObject::Instantiate("BackgroundOverlay")->AddComponent<UIImage>();
    bgOverlay->SetTexture("/cis460-minecraft/textures/ui/1px.tga");
    bgOverlay->GetTransform()->UISetSize(Engine::GetScreenSize());
    bgOverlay->GetGameObject()->GetComponent<UIRenderer>()->GetMaterial()->SetVector("Color", glm::vec4(0,0,0, .65f));
    gameObject->GetTransform()->AddChild(bgOverlay->GetGameObject()->GetTransform());

    background = GameObject::Instantiate("Background")->AddComponent<UIImage>();
    background->SetTexture("/cis460-minecraft/textures/ui/inventory_background.tga");
    background->GetTransform()->UISetSize(background->GetTransform()->UIGetSize() * 4.f);
    gameObject->GetTransform()->AddChild(background->GetGameObject()->GetTransform());

    for(int i = 0; i < 36; i++)
        AddSlot(i);

    for(int i = 0; i < 9; i++)
        AddCraftingSlot(i);

    AddCraftingSlot(9);

    this->dragItem = GameObject::Instantiate()->AddComponent<UIItem>();
    this->dragItem->GetGameObject()->GetTransform()->SetLocalPosition(glm::vec3(0,0,-30));// Need an easy z depth
    dragItem->SetItem(Item());

    // Add it after all other stuff
    itemDesc = GameObject::Instantiate("ItemDesc")->AddComponent<UIText>();
    itemDesc->SetTextSize(30);
    GetTransform()->AddChild(itemDesc->GetTransform());
}

void UIInventoryScreen::Update()
{
    glm::vec2 screenSize = glm::vec2(Engine::GetInstance()->GetContext()->width(), Engine::GetInstance()->GetContext()->height());
    glm::vec2 bgSize = background->GetTransform()->UIGetSize();

    glm::vec2 bgPos = glm::vec2(screenSize.x * .5f  - bgSize.x * .5, screenSize.y * .5f  - bgSize.y * .5);
    GetTransform()->UISetLocalPosition(bgPos);
    bgOverlay->GetTransform()->UISetLocalPosition(bgPos * -1.f);

    glm::vec2 cursorPosition = Engine::GetCurrentMousePosition();
    cursorPosition.y = screenSize.y - cursorPosition.y;
    this->dragItem->GetGameObject()->GetTransform()->SetLocalPosition(glm::vec3(cursorPosition.x, cursorPosition.y, 0));

    if(!this->hovered)
        itemDesc->GetGameObject()->SetEnabled(false);

    this->hovered = false;

    CheckCrafting();
}

void UIInventoryScreen::SetPlayer(Player *player)
{
    this->player = player;
}

void UIInventoryScreen::OnKeyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_E)
    {
        bool enable = !this->gameObject->IsEnabledInHierarchy();

        this->gameObject->SetEnabled(enable);
        Engine::LockCursor(!enable);

        this->player->GetGameObject()->GetComponent<PlayerInput>()->SetEnabled(!enable);
    }
}
