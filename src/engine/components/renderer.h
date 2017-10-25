#ifndef RENDERER_H
#define RENDERER_H

#include "../assets/mesh.h"
#include "../assets/texture.h"
#include "../../la.h"
#include "../component.h"


class Material;

// A layer of abstraction that let's us implement different renderers,
class Renderer : public Component
{
protected:
	Material * material;

	virtual ~Renderer();

public:
	virtual void Awake();
	virtual void Render(const glm::mat4& viewProj, const glm::vec4 &viewVec,
		const glm::vec4 &DLightDirVec, const glm::vec4 &DLightColor, const float &time,
		const float &daytime, Texture ** ShadowMap, const glm::mat4 &ShadowViewProj,
        const glm::mat4 *ShadowViewProjArray, float bShaderDebugMode) = 0;

	Material * GetMaterial();
	void SetMaterial(Material *material);
};

class MeshRenderer : public Renderer
{
protected:
	Mesh * mesh;

public:
	virtual void Awake();
	virtual void Render(const glm::mat4& viewProj, const glm::vec4 &viewVec,
		const glm::vec4 &DLightDirVec, const glm::vec4 &DLightColor, const float &time,
		const float &daytime, Texture ** ShadowMap, const glm::mat4 &ShadowViewProj,
        const glm::mat4 *ShadowViewProjArray, float bShaderDebugMode);


	void SetMesh(Mesh * mesh);
	Mesh * GetMesh();
};

class UIRenderer : public MeshRenderer
{
public:
	virtual void Awake();
	virtual ~UIRenderer();
	virtual void Render(const glm::mat4& viewProj);
	virtual void Render(const glm::mat4 &viewProj, const glm::vec4 &viewVec, const glm::vec4 &DLightDirVec, const glm::vec4 &DLightColor, const float &time, const float &daytime, Texture * ShadowMap, const glm::mat4 &ShadowViewProj);
};

#endif // RENDERER_H
