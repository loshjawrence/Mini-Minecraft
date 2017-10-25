#include "playerphysics.h"

void PlayerPhysics::Awake()
{
    this->gravity = 20.f;
    this->kinematic = false;
}

void PlayerPhysics::PhysicsUpdate()
{
    if(!kinematic)
    {
        Transform * trans = gameObject->GetTransform();
        float delta = Engine::DeltaTime();
        glm::vec3 currentPosition = trans->WorldPosition();

        // Just plain euler integration, we have no oscillations
        currentVelocity += glm::vec3(0, -gravity, 0) * delta;
        glm::vec3 expectedPosition = currentPosition + currentVelocity * delta;

        MoveTowards(expectedPosition);
    }
}

bool PlayerPhysics::CollideWithWorld(const glm::vec3& position)
{
    glm::vec3 cellPosition = glm::floor((position));// * glm::sign(position);

    bool result = false;

    int x = cellPosition.x;
    int y = cellPosition.y;
    int z = cellPosition.z;

    if(chunkManager->GetBlock(x,y,z) != -1)
        result = true;

    // Now check on the middle of the player, because it can be in 3 different cells at the same time
    y = (int)(glm::floor(position + glm::vec3(0,.9f, 0)).y);

    if(chunkManager->GetBlock(x,y,z) != -1)
        result = true;

    // Now check on the upper cube of the player (Some margin to prevent clipping!)
    y = (int)(glm::floor(position + glm::vec3(0, 1.8f, 0)).y);

    if(chunkManager->GetBlock(x,y,z) != -1)
        result = true;

    return result;
}

// This method moves the player, checking collisions with the world
void PlayerPhysics::MoveTowards(glm::vec3 newPosition)
{
    Transform * trans = gameObject->GetTransform();

    if(kinematic)
    {
        trans->SetWorldPosition(newPosition);
    }
    else
    {
        glm::vec3 currentPosition = trans->WorldPosition();
        glm::vec3 expectedPosition = newPosition;

        glm::vec3 direction = expectedPosition - currentPosition;

        // Clamp next position, as far as 1 cube
        if(glm::length(direction) > 0.f)
            expectedPosition = currentPosition + glm::normalize(direction) * glm::min(1.f, glm::length(direction));

        float collisionMargin = .4f; // To prevent clipping, etc

        glm::vec3 displX = glm::vec3(expectedPosition.x + collisionMargin * glm::sign(direction.x), currentPosition.y, currentPosition.z);
        glm::vec3 displY = glm::vec3(currentPosition.x, expectedPosition.y, currentPosition.z);
        glm::vec3 displZ = glm::vec3(currentPosition.x, currentPosition.y, expectedPosition.z + collisionMargin * glm::sign(direction.z));

        glm::vec3 result = expectedPosition;

        if(CollideWithWorld(displX))
        {
            result.x = currentPosition.x;
            currentVelocity.x = 0;
        }

        if(CollideWithWorld(displY))
        {
            result.y = currentPosition.y;
            currentVelocity.y = 0;
        }

        if(CollideWithWorld(displZ))
        {
            result.z = currentPosition.z;
            currentVelocity.z = 0;
        }

        trans->SetWorldPosition(result);
    }
}

void PlayerPhysics::SetVelocity(const glm::vec3 &velocity)
{
    this->currentVelocity = velocity;
}

void PlayerPhysics::SetKinematic(bool noclip)
{
    this->kinematic = noclip;
    this->currentVelocity = glm::vec3();
}
