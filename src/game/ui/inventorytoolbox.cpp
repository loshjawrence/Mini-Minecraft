#include "inventorytoolbox.h"
#include "../player.h"
#include "uiinventoryslot.h"

void UIInventoryToolbox::AddSlot(int slotNumber)
{
    UIInventorySlot * slot = GameObject::Instantiate("")->AddComponent<UIInventorySlot>();
    gameObject->GetTransform()->AddChild(slot->GetGameObject()->GetTransform());
    slot->Initialize(slotNumber,
                     player,
                     std::bind(&UIInventoryToolbox::OnSlotClicked, this, std::placeholders::_1),
                     std::bind(&UIInventoryToolbox::OnSlotClicked, this, std::placeholders::_1), false);
    slot->EnableBackground(false);

    glm::vec2 offset(4, 0);

    glm::vec2 p = ((selectedElement->GetTextureSize() - offset) * (float)slotNumber - offset * .25f) * 4.f;
    p.y = 5;
    p.x += 15;
    slot->GetTransform()->UISetLocalPosition(p);

    quickSlots.push_back(slot);
}

void UIInventoryToolbox::OnSlotClicked(int slot)
{
}

UIInventoryToolbox::~UIInventoryToolbox()
{
}

void UIInventoryToolbox::Awake()
{
    this->player = nullptr;
    this->backgroundImage = nullptr;
    this->selectedElement = nullptr;
}

void UIInventoryToolbox::Start()
{    
    backgroundImage = GameObject::Instantiate("ToolboxBackground")->AddComponent<UIImage>();
    backgroundImage->SetTexture("/cis460-minecraft/textures/ui/toolbox.tga");
    backgroundImage->GetTransform()->UISetSize(backgroundImage->GetTransform()->UIGetSize() * 4.f);
    gameObject->GetTransform()->AddChild(backgroundImage->GetGameObject()->GetTransform());

    selectedElement = GameObject::Instantiate("SelectedElement")->AddComponent<UIImage>();
    selectedElement->SetTexture("/cis460-minecraft/textures/ui/toolbox_selected.tga");
    selectedElement->GetTransform()->UISetSize(selectedElement->GetTransform()->UIGetSize() * 4.f);
    gameObject->GetTransform()->AddChild(selectedElement->GetGameObject()->GetTransform());

    SelectSlot(0);

    for(int i = 0; i < 9; i++)
        AddSlot(i);
}

void UIInventoryToolbox::Update()
{
    glm::vec2 screenSize = glm::vec2(Engine::GetInstance()->GetContext()->width(), Engine::GetInstance()->GetContext()->height());
    glm::vec2 bgSize = backgroundImage->GetTransform()->UIGetSize();

    glm::vec2 bgPos = glm::vec2(screenSize.x * .5f  - bgSize.x * .5, 0.0);

    this->GetTransform()->UISetLocalPosition(bgPos);
}

void UIInventoryToolbox::SetPlayer(Player * player)
{
    this->player = player;
    this->player->SetToolbox(this);
}

void UIInventoryToolbox::SelectSlot(int slot)
{
    if(slot >= 0 && slot < 9)
    {
        this->currentSlot = slot;

        glm::vec2 offset(4, 0);

        glm::vec2 p = ((selectedElement->GetTextureSize() - offset) * (float)currentSlot - offset * .25f) * 4.f;
        p.y = -5;
        selectedElement->GetTransform()->UISetLocalPosition(p);
    }
}
