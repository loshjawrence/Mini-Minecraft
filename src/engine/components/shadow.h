#ifndef SHADOW_H
#define SHADOW_H


#include "../assets/rendertargettexture.h"
#include "../gameobject.h"
#include <openglcontext.h>
#include "directionallight.h"
#include "../../engine/common.h"

class Shadow : public Component //: public OrthographicCamera
{
public:
	Shadow();
    virtual void Awake();

    glm::mat4 CrateProj(float w, float h, int index);
    void Initialize(OpenGLContext * openGLContext, float w01, float h01, float w02, float h02, float w03, float h03, DirectionalLight* pSun);
    glm::mat4 ComputeViewProjectionMatrix(glm::vec3 PlayerWorldPosition, int index);
    void UpdateInfo(glm::vec3 PlayerWorldPosition);

	float nearClip;
	float farClip;
	glm::vec4 backgroundColor;

	glm::mat4 m_RotMat;

	glm::mat4 shadowmapVP[3];
	glm::mat4 m_proj[3];
	glm::mat4 m_view;

	RenderTargetTexture m_ShadowMap[3];
	DirectionalLight *m_pSun;
};

#endif // SHADOW_H
