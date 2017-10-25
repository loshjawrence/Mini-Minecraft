#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H


#include "../engine/common.h"

class Player;

class PlayerInput : public Component, public InputListener
{
private:
    Player * player;
    glm::vec3 velocity;
    float speed;

    float verticalAngle;
    float horizontalAngle;
    bool noclip;

public:

    void EnableFly(bool fly);
    void SetSpeed(float speed);
    float GetSpeed();

    virtual void Awake();
    virtual void Update();

    void OnKeyPressEvent(QKeyEvent *e);
    void OnKeyReleaseEvent(QKeyEvent * e);
    void OnMouseMoveEvent(QMouseEvent * e);
    void OnMousePressEvent(QMouseEvent * e);
    void OnMouseWheelEvent(QWheelEvent *e);
};

#endif // PLAYERINPUT_H
