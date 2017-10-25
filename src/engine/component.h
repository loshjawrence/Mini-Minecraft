#ifndef COMPONENT_H
#define COMPONENT_H

#include "input.h"

class GameObject;
class Transform;

class Component
{
private:
    friend class GameObject;

    // This method is to prevent having to overload the constructor every time
    void SetParentGameObject(GameObject * gameObject);

protected:
    GameObject * gameObject;
    bool enabled;

    // NOTE: DO NOT IMPLEMENT CONSTRUCTORS DIRECTLY,
    // OR THE FACTORY PATTERN WONT WORK.
    // Use Awake for initialization.
    Component();

    // Note: This virtual destructor is important not only because of the destructor itself,
    // but because it forces each component to be polymorphic, thus making dynamic_cast work
    virtual ~Component();

    virtual void OnDestroy();

public:
    // Think of this method as the constructor
    virtual void Awake();

    // This method will be called on the first frame it is enabled.
    virtual void Start();

    // Called every frame
    virtual void Update();

    // Called after every update, before rendering
    virtual void PhysicsUpdate();

    void Destroy();

    void SetEnabled(bool enabled);
    bool IsEnabled();

    GameObject * GetGameObject();
    Transform * GetTransform();

    std::string ToString();
};

class ComponentExample : public Component
{
};

class ComponentWithInputExample : public Component, public InputListener
{
public:
    void OnKeyPressEvent(QKeyEvent *e) {}
    void OnMouseEvent(QMouseEvent *e) {}
};

#endif // COMPONENT_H
