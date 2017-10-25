#include "uibutton.h"

void UIButton::Awake()
{
    UIImage::Awake();

    hoverColor = glm::vec4(1,1,1,1);
    hoverTexture = nullptr;
    pressed = false;
    hover = false;
}

void UIButton::AddClickListener(std::function<void ()> f)
{
    this->onClickListeners.push_back(f);
}

void UIButton::AddHoverListener(std::function<void ()> f)
{
    this->onHoverListeners.push_back(f);
}

void UIButton::SetHoverTexture(Texture *t)
{
    this->hoverTexture = t;
}

void UIButton::SetHoverColor(glm::vec4 color)
{
    this->hoverColor = color;
}

void UIButton::Update()
{
    if(ContainsPoint(Engine::GetCurrentMousePosition()))
        for(auto l = onHoverListeners.begin(); l != onHoverListeners.end(); l++)
            (*l)(); // C++ u scary
}

void UIButton::OnMousePressEvent(QMouseEvent *e)
{
    bool isOver = ContainsPoint(Engine::GetCurrentMousePosition());

    if(isOver)
        pressed = true;
}

void UIButton::OnMouseReleaseEvent(QMouseEvent *e)
{
    bool isOver = ContainsPoint(Engine::GetCurrentMousePosition());

    if(isOver)
    {
        for(auto l = onClickListeners.begin(); l != onClickListeners.end(); l++)
            (*l)(); // C++ u scary
    }

    // Button is always unpressed, but if mouse was on top, it triggers the event
    pressed = false;
}

void UIButton::OnMouseMoveEvent(QMouseEvent *e)
{
    // No raycast for now, minecraft has a very simple UI with non overlapping elements
    bool isOver = ContainsPoint(Engine::GetCurrentMousePosition());

    if(hover != isOver)
    {
        if(hoverTexture != nullptr)
            this->material->SetTexture("MainTexture", isOver ? hoverTexture : texture);

        this->material->SetColor("Color", isOver ? hoverColor : color);
    }

    hover = isOver;
}
