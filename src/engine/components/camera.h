#ifndef CAMERA_H
#define CAMERA_H

#include "la.h"
#include "../component.h"

class Camera : public Component
{
protected:
    virtual ~Camera();

    // Computed
    float aspect;
    glm::mat4 viewProjectionMatrix;

    // Input
    int width;
    int height;
    float nearClip;
    float farClip;
    glm::vec4 backgroundColor;

    // Depends on each camera type!
    virtual glm::mat4 ComputeViewProjectionMatrix() = 0;

public:
    virtual void Awake();
    virtual void PhysicsUpdate();
    void UpdateScreenSize();

    const glm::mat4& GetViewProjectionMatrix();
    const glm::vec3 GetViewVector();
};

class PerspectiveCamera : public Camera
{
protected:
    float fieldOfView;
    virtual glm::mat4 ComputeViewProjectionMatrix();

public:
    virtual void Awake();
};

class OrthographicCamera: public Camera
{
    virtual glm::mat4 ComputeViewProjectionMatrix();
};

class UICamera: public Camera
{
public:
    virtual void Awake();
    virtual ~UICamera();
    virtual glm::mat4 ComputeViewProjectionMatrix();
};

#endif // CAMERA_H
