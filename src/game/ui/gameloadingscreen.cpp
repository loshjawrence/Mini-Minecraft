#include "gameloadingscreen.h"


void GameLoadingScreen::Start()
{
    this->background = GameObject::Instantiate()->AddComponent<UIImage>();
    this->logo = GameObject::Instantiate()->AddComponent<UIImage>();

    logo->SetTexture("/cis460-minecraft/textures/ui/logo.tga");
    logo->CenterOnScreen();

    background->SetTexture("/cis460-minecraft/textures/ui/logo_background.tga");
    background->GetTransform()->UISetSize(Engine::GetScreenSize());

    GetTransform()->AddChild(background->GetTransform());
    GetTransform()->AddChild(logo->GetTransform());
}

void GameLoadingScreen::Update()
{
    this->timer += Engine::DeltaTime();

    // Lets hide some latency
    if(timer > 6.f)
        this->gameObject->SetEnabled(false);
}
