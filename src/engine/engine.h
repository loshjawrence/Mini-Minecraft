#ifndef ENGINE_H
#define ENGINE_H

#include "../openglcontext.h"
#include "log.h"
#include "components/camera.h"
#include "components/renderer.h"
//#include "components/directionallight.h"

#include "assets/texture.h"
#include <vector>

class GameObject;
class Component;
class Input;

typedef std::vector<GameObject*>::iterator GameObjectIterator;
typedef std::vector<Camera*>::iterator CameraIterator;
typedef std::vector<Renderer*>::iterator RendererIterator;
typedef std::vector<UIRenderer*>::iterator UIRendererIterator;

// Basic engine inspired by Unity's entity-component architecture
class Engine
{
private:
    friend class GameObject;
    friend class Camera; // There's some cooperation needed with cameras that is simpler this way
    friend class Renderer;
    friend class UIRenderer;

    static Engine * instance;

    std::vector<GameObject*> gameObjects; // TODO: make pool
    std::vector<Camera*> cameras;
    std::vector<Renderer*> renderers;

    // UI related, because it doesnt make sense to have Unity's Canvas thing right now
    std::vector<UIRenderer*> uiRenderers;
    UICamera * uiCamera;

    // These lists are to prevent concurrent modification of the main list
    // while updating the frame
    std::vector<GameObject*> gameObjectsToAdd;
    std::vector<GameObject*> gameObjectsToDelete;

    Input * input;
    ConcurrentLog<MultiLog> * log;

    OpenGLContext * openGLContext;

    float time;
    float deltaTime;

    bool lockCursor;



    bool bDebugShaderMode;

    Engine();

    void RegisterRenderer(Renderer * renderer);
    void DeregisterRenderer(Renderer * renderer);

    void RegisterUIRenderer(UIRenderer * renderer);
    void DeregisterUIRenderer(UIRenderer * renderer);


    void RegisterCamera(Camera* camera);
    void DeregisterCamera(Camera* camera);

    void RegisterGameObject(GameObject * o);
    void DeleteGameObject(GameObject * o);

    void InitializeNewGameObjects();
    void DeleteGameObjects();

    void RenderUI();
    void RenderShadow(glm::mat4 viewProj, glm::vec4 ViewVector, float DebugShaderMode);
public:

    // If we wanted something more portable, this context should be
    // decoupled in an additional Rendering Module or similar
    void Initialize(OpenGLContext * openGLContext);
    void Update(float deltaTime);
    void Render();

    void OnOpenGLContextChanged();

    OpenGLContext * GetContext();
    Input * GetInput();

    void Test();

    void AddLogger(Log * logger);

    static Engine* GetInstance();
    static Log* GetLog();
    static float Time();
    static float DeltaTime();

    static glm::vec2 GetScreenSize();
    static glm::vec2 GetCurrentMousePosition();
    static void LockCursor(bool lockCursor);
    static bool IsCursorLocked();

    // Simplified logging
    static void LogVerbose(const std::string &str);
    static void LogDebug(const std::string &str);
    static void LogInfo(const std::string &str);
    static void LogWarning(const std::string &str);
    static void LogError(const std::string &str);



    void DebugShade()
    {
        bDebugShaderMode = !bDebugShaderMode;
    }



    Camera* GetMainCamera()
    {
        return cameras[0];
    }

    //DirectionalLight m_DirectionalLight;

    glm::vec4 m_DirectionalLightDir;
    glm::vec4 m_DirectionalLightCol;
    float m_DirectionalLightDaytime;

	glm::mat4 shadowmapVP[3];

	GLuint m_ShadowFrameBuffer[3];
	GLuint m_ShadowRenderBuffer[3];
    Texture * m_ShadowMapResource[3];
};

#endif // ENGINE_H
