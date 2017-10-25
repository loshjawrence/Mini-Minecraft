#include "renderer.h"
#include "../material.h"
#include "../gameobject.h"

Renderer::~Renderer()
{
    Engine::GetInstance()->DeregisterRenderer(this);
}

void Renderer::Awake()
{
    this->material = nullptr;
    Engine::GetInstance()->RegisterRenderer(this);
}

Material *Renderer::GetMaterial()
{
    return material;
}

void Renderer::SetMaterial(Material *material)
{
    this->material = material;
}

void MeshRenderer::SetMesh(Mesh *mesh)
{
    this->mesh = mesh;
}

Mesh *MeshRenderer::GetMesh()
{
    return mesh;
}

void MeshRenderer::Awake()
{
    Renderer::Awake();
    this->mesh = nullptr;
}

void MeshRenderer::Render(const glm::mat4 &viewProj, const glm::vec4 &viewVec,
    const glm::vec4 &DLightDirVec, const glm::vec4 &DLightColor, const float &time, const float &daytime,
    Texture **ShadowMap, const glm::mat4 &ShadowViewProj, const glm::mat4 *ShadowViewProjArray, float bShaderDebugMode)
{
    if(this->material != nullptr && this->mesh != nullptr)
    {
        this->material->SetVector("ViewVec", viewVec); //need more simple way
        this->material->SetVector("DLightDirVec", DLightDirVec); //need more simple way
        this->material->SetVector("DLightColor", DLightColor); //need more simple way
		this->material->SetMatrix("ShadowViewProj", ShadowViewProj);
        this->material->SetFloat("Time", time);  //need more simple way
        this->material->SetFloat("DayTime", daytime); //need more simple way
        this->material->SetFloat("DebugMode", bShaderDebugMode); //need more simple way

        this->material->SetMatrix("ShadowViewProjArray01", ShadowViewProjArray[0]);
        this->material->SetMatrix("ShadowViewProjArray02", ShadowViewProjArray[1]);
        this->material->SetMatrix("ShadowViewProjArray03", ShadowViewProjArray[2]);

        if(ShadowMap != nullptr)
        {
            if(ShadowMap[0] != nullptr)
                this->material->SetTexture("ShadowMap01", ShadowMap[0]);
            if(ShadowMap[1] != nullptr)
                this->material->SetTexture("ShadowMap02", ShadowMap[1]);
            if(ShadowMap[2] != nullptr)
                this->material->SetTexture("ShadowMap03", ShadowMap[2]);
        }



        Transform * t = this->gameObject->GetTransform();
        this->material->Render(mesh, viewProj, t->T(), t->invT(), t->invTransT());
    }
}

void UIRenderer::Awake()
{
    // Override registration for base renderer, as we need to be on other pass
    Engine::GetInstance()->RegisterUIRenderer(this);
    this->mesh = nullptr;
    this->material = nullptr;
}

UIRenderer::~UIRenderer()
{
    Engine::GetInstance()->DeregisterUIRenderer(this);
}

void UIRenderer::Render(const glm::mat4 &viewProj)
{
    if(this->material != nullptr && this->mesh != nullptr)
    {
        Transform * t = this->gameObject->GetTransform();
        this->material->Render(mesh, viewProj, t->T(), t->invT(), t->invTransT());
    }
}

void UIRenderer::Render(const glm::mat4 &viewProj, const glm::vec4 &viewVec, const glm::vec4 &DLightDirVec, const glm::vec4 &DLightColor, const float &time, const float &daytime, Texture *ShadowMap, const glm::mat4 &ShadowViewProj)
{
    // Do nothing
}
