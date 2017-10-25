#ifndef SKYBOX_H
#define SKYBOX_H

#include "../assets/texture.h"
#include "../gameobject.h"
#include <openglcontext.h>

#include "../../engine/common.h"

class SkyBox : public Component
{
public:
    virtual void Awake();

protected:
    void InitializeCubeTexture();
    Mesh* BuildCube();
    CubeTexture m_CubeTexture;
};

#endif // SKYBOX_H
