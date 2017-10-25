#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <la.h>
#include "mesh.h"
#include "texture.h"

// Just a collection of common meshes we need
class MeshFactory
{
public:
    static Mesh * BuildCube(bool interleave, bool writeOnly = true);
    static Mesh * BuildWireCube();
    static Mesh * BuildCross();
    static Mesh * BuildQuad(bool writeOnly = true);
    static Mesh * BuildMeshFromTexture(ReadableTexture * texture);
};

#endif // MESHFACTORY_H
