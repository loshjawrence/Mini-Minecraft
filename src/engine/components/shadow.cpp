#include "shadow.h"

static const int CUB_IDX_COUNT = 6;
static const int CUB_VERT_COUNT = 4;

Shadow::Shadow()
{

}


void Shadow::Awake()
{
    MeshRenderer * renderer = this->gameObject->AddComponent<MeshRenderer>();

    Material * material = new Material("shadow");

    material->SetOverrideDrawingMode(GL_TRIANGLES);
    material->SetFeature(GL_DEPTH_TEST, true);
    material->SetRenderingQueue(Material::Background);

    renderer->SetMaterial(material);
}

glm::mat4 Shadow::CrateProj(float w, float h, int index)
{
    nearClip = DISTANCE_FROM_SUN*0.9;
    farClip = DISTANCE_FROM_SUN + DISTANCE_FROM_SUN*0.1;

    glm::mat4 proj;

    double Range = 1.0 / (farClip - nearClip);
    proj[0][0] = 2.0 / w;
    proj[0][1] = 0.0;
    proj[0][2] = 0.0;
    proj[0][3] = 0.0;

    proj[1][0] = 0.0;
    proj[1][1] = 2.0 / h;
    proj[1][2] = 0.0;
    proj[1][3] = 0.0;

    proj[2][0] = 0.0;
    proj[2][1] = 0.0;
    proj[2][2] = Range;
    proj[2][3] = 0.0;

    proj[3][0] = 0.0;
    proj[3][1] = 0.0;
    proj[3][2] = -Range * nearClip;
    proj[3][3] = 1.0;

    return proj;
}

void Shadow::Initialize(OpenGLContext * openGLContext, float w01, float h01, float w02, float h02, float w03, float h03, DirectionalLight* pSun)
{

    m_ShadowMap[0].Create(ShadowMapResolution01, ShadowMapResolution01, openGLContext);
    m_ShadowMap[1].Create(ShadowMapResolution02, ShadowMapResolution02, openGLContext);
    m_ShadowMap[2].Create(ShadowMapResolution03, ShadowMapResolution03, openGLContext);


    m_proj[0] = CrateProj(w01, h01, 0);
    m_proj[1] = CrateProj(w02, h02, 1);
    m_proj[2] = CrateProj(w03, h03, 2);

    m_pSun = pSun;

    shadowmapVP[0] = ComputeViewProjectionMatrix(glm::vec3(0, 0, 0), 0);
    shadowmapVP[1] = ComputeViewProjectionMatrix(glm::vec3(0, 0, 0), 0);
    shadowmapVP[2] = ComputeViewProjectionMatrix(glm::vec3(0, 0, 0), 0);
}

glm::mat4 Shadow::ComputeViewProjectionMatrix(glm::vec3 PlayerWorldPosition, int index)
{
    glm::vec3 forward = glm::vec3(m_pSun->m_direction);
    glm::vec3 right = glm::normalize(glm::vec3(-1, 0, 1));
    glm::vec3 up = glm::cross(forward, right);

    glm::vec3 SunPosition = glm::vec3(-m_pSun->m_direction.x*DISTANCE_FROM_SUN, -m_pSun->m_direction.y*DISTANCE_FROM_SUN,
        -m_pSun->m_direction.z*DISTANCE_FROM_SUN) + glm::vec3(PlayerWorldPosition.x, PlayerWorldPosition.y, PlayerWorldPosition.z);

    glm::mat4 view;

    float x = -1.0f*glm::dot(SunPosition, right);
    float y = -1.0f*glm::dot(SunPosition, up);
    float z = -1.0f*glm::dot(SunPosition, forward);

    /// colums!!
    view[0][0] = right[0];
    view[1][0] = right[1];
    view[2][0] = right[2];
    view[3][0] = x;

    view[0][1] = up[0];
    view[1][1] = up[1];
    view[2][1] = up[2];
    view[3][1] = y;

    view[0][2] = forward[0];
    view[1][2] = forward[1];
    view[2][2] = forward[2];
    view[3][2] = z;

    view[0][3] = 0.0f;
    view[1][3] = 0.0f;
    view[2][3] = 0.0f;
    view[3][3] = 1.0f;


    m_view = view;

    return m_proj[index] * m_view;
}

void Shadow::UpdateInfo(glm::vec3 PlayerWorldPosition)
{
    shadowmapVP[0] = ComputeViewProjectionMatrix(PlayerWorldPosition, 0);
    shadowmapVP[1] = ComputeViewProjectionMatrix(PlayerWorldPosition, 1);
    shadowmapVP[2] = ComputeViewProjectionMatrix(PlayerWorldPosition, 2);
}
