#include "camera.h"
#include "../gameobject.h"

Camera::~Camera()
{
    Engine::GetInstance()->DeregisterCamera(this);
}

void Camera::Awake()
{
    this->aspect = 1.f;
    this->viewProjectionMatrix = glm::mat4(1.f);
    this->width = Engine::GetInstance()->GetContext()->width();
    this->height = Engine::GetInstance()->GetContext()->height();
    this->nearClip = .1f;
    this->farClip = 1000000.f;
    this->backgroundColor = glm::vec4(.2f, .2f, .2f, 1.f);

    Engine::GetInstance()->RegisterCamera(this);
}

void Camera::PhysicsUpdate()
{
    this->aspect = width / (float)height;
    this->viewProjectionMatrix = ComputeViewProjectionMatrix();
}

void Camera::UpdateScreenSize()
{
    this->width = Engine::GetInstance()->GetContext()->width();
    this->height = Engine::GetInstance()->GetContext()->height();

    PhysicsUpdate();
}

const glm::mat4 &Camera::GetViewProjectionMatrix()
{
    return viewProjectionMatrix;
}

const glm::vec3 Camera::GetViewVector()
{
    Transform * trans = this->gameObject->GetTransform();
    return  trans->Forward();
}

glm::mat4 PerspectiveCamera::ComputeViewProjectionMatrix()
{
    Transform * trans = this->gameObject->GetTransform();

    glm::vec3 cameraPosition = trans->WorldPosition();
    glm::vec3 forward = trans->Forward();
    glm::vec3 up = trans->Up();
    glm::vec3 target = cameraPosition + forward;

    return glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip) * glm::lookAt(cameraPosition, target, up);
}

void PerspectiveCamera::Awake()
{
    Camera::Awake();
    this->fieldOfView = 75.f;
}

glm::mat4 OrthographicCamera::ComputeViewProjectionMatrix()
{
    Transform * trans = this->gameObject->GetTransform();

    glm::vec3 cameraPosition = trans->WorldPosition();
    glm::vec3 forward = trans->Forward();
    glm::vec3 up = trans->Up();
    glm::vec3 target = cameraPosition + forward;

    return glm::ortho(0.f, (float)width, 0.f, (float)height, nearClip, farClip) * glm::lookAt(cameraPosition, target, up);
}

void UICamera::Awake()
{
    this->aspect = 1.f;
    this->viewProjectionMatrix = glm::mat4(1.f);
    this->width = Engine::GetInstance()->GetContext()->width();
    this->height = Engine::GetInstance()->GetContext()->height();
    this->nearClip = .1f;
    this->farClip = 1000000.f;
    this->backgroundColor = glm::vec4(.2f, .2f, .2f, 1.f);
}

UICamera::~UICamera()
{
}

glm::mat4 UICamera::ComputeViewProjectionMatrix()
{
    return glm::ortho(0.f, (float)width, 0.f, (float)height, nearClip, farClip) * glm::translate(glm::mat4(1), glm::vec3(0,0,-1)); // We need to offset Z a bit
}
