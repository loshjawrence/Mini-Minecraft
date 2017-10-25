#include "playercross.h"

void PlayerCross::Awake()
{
    MeshRenderer * renderer = this->gameObject->AddComponent<MeshRenderer>();
    renderer->SetMesh(MeshFactory::BuildCross());

    // Make sure it's being drawn at the front, and with no depth testing!
    Material * material = new Material("cross");
    material->SetVector("Color", glm::vec4(.5, .5, .5, 1.0));
    material->SetOverrideDrawingMode(GL_LINES);
    material->SetFeature(GL_DEPTH_TEST, false);
    material->SetRenderingQueue(Material::Overlay);

    renderer->SetMaterial(material);
}
