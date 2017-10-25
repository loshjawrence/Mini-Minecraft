#include "playercubehighlight.h"

void PlayerCubeHighlight::Awake()
{
    MeshRenderer * renderer = this->gameObject->AddComponent<MeshRenderer>();
    renderer->SetMesh(MeshFactory::BuildWireCube());

    // Make sure it's being drawn at the front, and with no depth testing!
    Material * material = new Material("cross");
    material->SetVector("Color", glm::vec4(.25, .25, .25, 1.0));
    material->SetOverrideDrawingMode(GL_LINES);
    material->SetFeature(GL_DEPTH_TEST, false);
    material->SetRenderingQueue(Material::Overlay);

    renderer->SetMaterial(material);
}
