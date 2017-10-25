#include "skybox.h"

void SkyBox::Awake()
{
    InitializeCubeTexture();

    MeshRenderer * renderer = this->gameObject->AddComponent<MeshRenderer>();
    renderer->SetMesh(BuildCube());

    // Make sure it's being drawn at the front, and with no depth testing!
    Material * material = new Material("skybox");

    material->SetCubeTexture("CubeMap01", m_CubeTexture.m_Texture[_0H]);
    material->SetCubeTexture("CubeMap02", m_CubeTexture.m_Texture[_6H]);
    material->SetCubeTexture("CubeMap03",m_CubeTexture.m_Texture[_12H]);
    material->SetCubeTexture("CubeMap04",m_CubeTexture.m_Texture[_18H]);

    material->SetFeature(GL_DEPTH_TEST, false);
    material->SetRenderingQueue(Material::Background);

    renderer->SetMaterial(material);

    this->gameObject->GetTransform()->SetLocalScale(glm::vec3(100000.0f));
}

void SkyBox::InitializeCubeTexture()
{
    std::vector<const char*> _0h;
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_posX.tga");
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_negX.tga");
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_posY.tga");
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_negY.tga");
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_posZ.tga");
    _0h.push_back("/cis460-minecraft/textures/cubemaps/_0h_negZ.tga");
    m_CubeTexture.LoadFromImage(_0h, _0H);

    std::vector<const char*> _6h;
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_posX.tga");
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_negX.tga");
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_posY.tga");
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_negY.tga");
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_posZ.tga");
    _6h.push_back("/cis460-minecraft/textures/cubemaps/_6h_negZ.tga");
    m_CubeTexture.LoadFromImage(_6h, _6H);

    std::vector<const char*> _12h;
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_posX.tga");
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_negX.tga");
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_posY.tga");
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_negY.tga");
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_posZ.tga");
    _12h.push_back("/cis460-minecraft/textures/cubemaps/_12h_negZ.tga");
    m_CubeTexture.LoadFromImage(_12h, _12H);

    std::vector<const char*> _18h;
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_posX.tga");
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_negX.tga");
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_posY.tga");
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_negY.tga");
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_posZ.tga");
    _18h.push_back("/cis460-minecraft/textures/cubemaps/_18h_negZ.tga");
    m_CubeTexture.LoadFromImage(_18h, _18H);
}

Mesh* SkyBox::BuildCube()
{
    Mesh * output = MeshFactory::BuildCube(false);
    output->SetWriteOnly(true); // So we don't worry about these arrays being leaked...
    output->Upload();
    return output;
}
