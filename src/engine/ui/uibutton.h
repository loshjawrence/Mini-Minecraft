#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "uiimage.h"
#include "../material.h"
#include "../assets/mesh.h"
#include "../components/renderer.h"

#include <vector>
#include <functional>

class UIButton : public UIImage, public InputListener
{
protected:
    bool pressed;
    bool hover;
    Texture * hoverTexture;
    glm::vec4 hoverColor;

    std::vector<std::function<void()>> onClickListeners;
    std::vector<std::function<void()>> onHoverListeners;

public:
    virtual void Awake();
    void AddClickListener(std::function<void()> f);
    void AddHoverListener(std::function<void()> f); // Called every frame the mouse hovers over the mouse

    void SetHoverTexture(Texture * t);
    void SetHoverColor(glm::vec4 color);

    void Update();

    void OnMousePressEvent(QMouseEvent *e);
    void OnMouseReleaseEvent(QMouseEvent *e);
    void OnMouseMoveEvent(QMouseEvent *e);
};

#endif // UIBUTTON_H
