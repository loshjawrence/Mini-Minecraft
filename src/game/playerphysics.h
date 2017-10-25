#ifndef PLAYERPHYSICS_H
#define PLAYERPHYSICS_H

#include "../engine/common.h"
#include "chunkmanager.h"

class PlayerPhysics : public Component
{
private:

    float gravity;
    bool kinematic;

    glm::vec3 currentVelocity;

    bool CollideWithWorld(const glm::vec3 &position);

public:

    ChunkManager * chunkManager;

    void Awake();

    void PhysicsUpdate();

    void MoveTowards(glm::vec3 newPosition);
    void SetVelocity(const glm::vec3& velocity);
    void SetKinematic(bool kinematic);
};

#endif // PLAYERPHYSICS_H
