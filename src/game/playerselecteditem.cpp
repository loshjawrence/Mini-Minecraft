#include "playerselecteditem.h"

#define TARGET_ANGLE glm::radians(-75.f)

void PlayerSelectedItem::Awake()
{
    this->timer = 0.f;
    this->momentum = 0.f;
    this->animating = false;

    this->currentItemView = GameObject::Instantiate("ItemContainer")->AddComponent<ItemView>();
    this->GetTransform()->AddChild(currentItemView->GetTransform());
    SelectItem(Item());
}

void PlayerSelectedItem::Update()
{
    if(animating)
    {
        this->timer += Engine::DeltaTime();
        float duration = .21f;

        if(timer < duration)
        {
            float t = (timer / duration);
            float angle = glm::radians(glm::sin(t * 2 * glm::pi<float>()) * 90.f) * glm::mix(1.f, 0.f, t);
            this->currentItemView->GetTransform()->SetLocalRotation(glm::vec3(0, 0, angle));

            float displ = glm::sin(t * 2 * glm::pi<float>()) * (currentItemView->GetItem().type == Item::BLOCK ? .1f : 1.5f);
            glm::vec3 p =this->currentItemView->GetTransform()->LocalPosition();
            this->currentItemView->GetTransform()->SetLocalPosition(glm::vec3(0, 0, p.z - displ));
        }
        else
        {
            timer = 0.f;
            animating = false;
            this->currentItemView->GetTransform()->SetLocalPosition(glm::vec3());
        }
    }
    else
    {
        // Minecraft actually has another transform that lerps into the current orientation... but
        // no time left!
        // Decay momentum
        this->momentum *= .5f;
        float angle = glm::radians(momentum * 45.f);
        GetTransform()->SetLocalRotation(glm::vec3(angle * .25f, TARGET_ANGLE + angle, 0));
    }
}

void PlayerSelectedItem::StartAnimation()
{
    this->animating = true;
    this->timer = 0.f;
    this->currentItemView->GetTransform()->SetLocalPosition(glm::vec3());
}

void PlayerSelectedItem::AddMomentum(float direction)
{
    this->momentum += direction * .001f;
    this->momentum = glm::clamp(momentum, -1.f, 1.f);
}

void PlayerSelectedItem::SelectItem(Item item)
{
    this->currentItemView->SetItem(item);

    if(item.type == Item::BLOCK)
    {
        GetTransform()->SetLocalPosition(glm::vec3(-1, -.75, 1));
        GetTransform()->SetLocalScale(glm::vec3(.6f));
        GetTransform()->SetLocalRotation(glm::vec3(0, TARGET_ANGLE, 0));
    }
    else
    {
        GetTransform()->SetLocalPosition(glm::vec3(-1, -.45, 1));
        GetTransform()->SetLocalScale(glm::vec3(1.15, 1.15, .115));
        GetTransform()->SetLocalRotation(glm::vec3(0, TARGET_ANGLE, 0));
    }
}
