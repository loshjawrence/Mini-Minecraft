#include "engine.h"
#include "gameobject.h"
#include "components/camera.h"
#include "components/renderer.h"
#include "material.h"
#include <sstream>
#include "common.h"

Engine * Engine::instance = nullptr;

Engine::Engine() : gameObjects(), cameras(), gameObjectsToAdd(),
    gameObjectsToDelete(), input(new Input()),
    log(new ConcurrentLog<MultiLog>(new MultiLog())), openGLContext(nullptr), time(0.f), deltaTime(0.f)
{
    this->log->GetInternalLogger()->AddLogger(new Log(&std::cout));
    this->uiCamera = nullptr;
    this->lockCursor = true;


    bDebugShaderMode = false;


    //m_DirectionalLight.Update(14);
}

void Engine::RegisterRenderer(Renderer *renderer)
{
//    LogVerbose("Registering renderer...");
    this->renderers.push_back(renderer);
}

void Engine::DeregisterRenderer(Renderer *renderer)
{
//    LogVerbose("Deregistering renderer...");
    this->renderers.erase(std::find(renderers.begin(), renderers.end(), renderer));
}

void Engine::RegisterUIRenderer(UIRenderer *renderer)
{
    this->uiRenderers.push_back(renderer);
}

void Engine::DeregisterUIRenderer(UIRenderer *renderer)
{
    this->uiRenderers.erase(std::find(uiRenderers.begin(), uiRenderers.end(), renderer));
}

void Engine::RegisterCamera(Camera *camera)
{
    LogVerbose("Registering camera...");
    this->cameras.push_back(camera);
}

void Engine::DeregisterCamera(Camera *camera)
{
    // There's no concurrency problem here
    LogVerbose("Deregistering camera...");
    this->cameras.erase(std::find(cameras.begin(), cameras.end(), camera));
}

void Engine::RegisterGameObject(GameObject *o)
{
    this->gameObjectsToAdd.push_back(o);
}

void Engine::DeleteGameObject(GameObject *o)
{
    this->gameObjectsToDelete.push_back(o);
}

// This must be called before the frame is updated
void Engine::InitializeNewGameObjects()
{
    for(GameObjectIterator g = gameObjectsToAdd.begin(); g != gameObjectsToAdd.end(); g++)
        gameObjects.push_back(*g);

    gameObjectsToAdd.clear();
}

// This must be called after the frame is updated
void Engine::DeleteGameObjects()
{
    for(GameObjectIterator g = gameObjectsToDelete.begin(); g != gameObjectsToDelete.end(); g++)
    {
        gameObjects.erase(std::find(gameObjects.begin(), gameObjects.end(), *g));
        delete (*g);
    }

    gameObjectsToDelete.clear();
}


void Engine::RenderShadow(glm::mat4 viewProj, glm::vec4 ViewVector, float DebugShaderMode)
{


    for (uint i = 0; i < SHADOWMAPCOUNT; i++)
    {

        openGLContext->glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFrameBuffer[i]);

        if (i == 0)
        {
            openGLContext->glViewport(0, 0, ShadowMapResolution01, ShadowMapResolution01);
        }
        else if (i == 1)
        {
            openGLContext->glViewport(0, 0, ShadowMapResolution02, ShadowMapResolution02);
        }
        else if (i == 2)
        {
            openGLContext->glViewport(0, 0, ShadowMapResolution03, ShadowMapResolution03);
        }

        openGLContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        openGLContext->glClearColor(-1, 0, 0, 0.0);

        for (RendererIterator r = renderers.begin(); r != renderers.end(); r++)
        {
            Renderer * renderer = *r;
            MeshRenderer * shadowrenderer;

            if (renderer->GetGameObject()->IsEnabledInHierarchy() && renderer->IsEnabled())
            {
                //Get shadowrenderer
                if (renderer->GetMaterial()->GetShader()->GetName() == "shadow")
                {
                    shadowrenderer = (MeshRenderer *)renderer;
                }
                //Draw shadowmap
                else if (renderer->GetMaterial()->GetShader()->GetName() == "lambert")
                {

                    Mesh* m = ((MeshRenderer *)renderer)->GetMesh();
                    shadowrenderer->SetMesh(m);
                    shadowrenderer->Render(viewProj, ViewVector,
                        m_DirectionalLightDir, m_DirectionalLightCol, this->time,
                        m_DirectionalLightDaytime, nullptr, shadowmapVP[i], shadowmapVP, DebugShaderMode);
                }
            }
        }

        openGLContext->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Engine::RenderUI()
{
    Camera * camera = uiCamera;
    glm::mat4 viewProj = camera->GetViewProjectionMatrix();

    for (UIRendererIterator r = uiRenderers.begin(); r != uiRenderers.end(); r++)
    {
        UIRenderer * renderer = *r;

        if (renderer->GetGameObject()->IsEnabledInHierarchy() && renderer->IsEnabled())
            renderer->Render(viewProj);
    }
}

void Engine::Initialize(OpenGLContext *openGLContext)
{
    this->openGLContext = openGLContext;
    this->uiCamera = GameObject::Instantiate("UICamera")->AddComponent<UICamera>();
}

void Engine::Update(float deltaTime)
{
    deltaTime = glm::clamp(deltaTime, 0.0f, .05f); // At most, 10fps internally
    this->time += deltaTime;
    this->deltaTime = deltaTime;

    log->FlushAll();

    // Make sure time has passed!
    if(deltaTime > .00001f)
    {
        InitializeNewGameObjects();

        // For now, objects are updated disregarding their hierarchy. When/if needed,
        // we can implement this
        for(GameObjectIterator g = gameObjects.begin(); g!= gameObjects.end(); g++)
            if((*g)->IsEnabledInHierarchy())
                (*g)->Update();

        // Update physics now... (TODO: maybe use the same pattern as Render but with a physics component?)
        // (Also, our physics is tied to the framerate, so careful with those calculations!)
        for(GameObjectIterator g = gameObjects.begin(); g!= gameObjects.end(); g++)
            if((*g)->IsEnabledInHierarchy())
                (*g)->PhysicsUpdate();

        DeleteGameObjects();
    }

    // Rendering is independent on time
    Render();
}

bool CompareRenderers(Renderer * lhs, Renderer * rhs)
{
    return Material::Compare(*lhs->GetMaterial(), *rhs->GetMaterial());
}

void Engine::Render()
{
	// Sort renderers by their material queues (TODO: Do something more optimized in case we have many renderers)
	std::sort(renderers.begin(), renderers.end(), CompareRenderers);

	// For now, this is a formality, as we don't actually have multiple viewports... (TODO)
	for (CameraIterator c = cameras.begin(); c != cameras.end(); c++)
	{
		Camera * camera = *c;
        camera->UpdateScreenSize();
		glm::mat4 viewProj = camera->GetViewProjectionMatrix();


        float DebugShaderMode = 0.0;

        if(bDebugShaderMode)
            DebugShaderMode = 1.0;

        //Draw ShadowMap
        RenderShadow(viewProj, glm::vec4(camera->GetViewVector(), 0.0), DebugShaderMode);

        //Render Objects
		openGLContext->glBindFramebuffer(GL_FRAMEBUFFER, openGLContext->defaultFramebufferObject());
		openGLContext->glViewport(0, 0, openGLContext->width(), openGLContext->height());

		openGLContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		openGLContext->glClearColor(0.2, 0.0, 0.0, 0.0);

		for (RendererIterator r = renderers.begin(); r != renderers.end(); r++)
		{
			Renderer * renderer = *r;

            if (renderer->GetGameObject()->IsEnabledInHierarchy() && renderer->IsEnabled())
			{
                if (renderer->GetMaterial()->GetShader()->GetName() != "shadow")
                {
                    renderer->Render(viewProj, glm::vec4(camera->GetViewVector(), 0.0),
                        m_DirectionalLightDir, m_DirectionalLightCol, this->time,
                        m_DirectionalLightDaytime, m_ShadowMapResource, shadowmapVP[0], shadowmapVP, DebugShaderMode);
				}
			}
		}
	}   

    // Now render the UI
    RenderUI();
}

void Engine::OnOpenGLContextChanged()
{
    for(CameraIterator c = cameras.begin(); c != cameras.end(); c++)
        (*c)->UpdateScreenSize();

    uiCamera->UpdateScreenSize();
}

OpenGLContext *Engine::GetContext()
{
    return openGLContext;
}

Input *Engine::GetInput()
{
    return input;
}

void Engine::Test()
{
//    GameObject * player = GameObject::Instantiate("Player");
//    Component * c1 = player->AddComponent<Component>();
//    Component * c2 = player->AddComponent<ComponentExample>();
//    Component * c3 = player->AddComponent<ComponentWithInputExample>();

//    Component * e1 = player->GetComponent<Component>();
//    Component * e2 = player->GetComponent<ComponentExample>();
//    Component * e3 = player->GetComponent<ComponentWithInputExample>();

//    assert(c1 == e1);
//    assert(c2 == e2);
//    assert(c3 == e3);
//    assert(typeid(*c2) == typeid(*e2));
//    assert(typeid(*c3) == typeid(*e3));

//    LogDebug(player->ToString());

////    std::cout << c1 << ", " << e1 << std::endl;
////    std::cout << c2 << ", " << e2 << std::endl;
////    std::cout << c3 << ", " << e3 << std::endl;
////    std::cout << typeid(*c2).name() << ", " << typeid(*e2).name() << std::endl;
////    std::cout << typeid(*c3).name() << ", " << typeid(*e3).name() << std::endl;

//    player->Destroy();
//    this->Update(1.0/60.0);
//    assert(this->gameObjects.size() == 0);

//    GameObject * cameraContainer = GameObject::Instantiate("Camera");
//    cameraContainer->AddComponent<PerspectiveCamera>();
//    cameraContainer->Destroy();
//    this->Update(1.0/60.0);
//    assert(this->gameObjects.size() == 0);
//    assert(this->cameras.size() == 0);
}

void Engine::AddLogger(Log *logger)
{
    this->log->GetInternalLogger()->AddLogger(logger);
}

// Note: NOT thread safe!
Engine *Engine::GetInstance()
{
    if(instance == nullptr)
        instance = new Engine();

    return instance;
}

Log *Engine::GetLog()
{
    return GetInstance()->log;
}

float Engine::Time()
{
    return GetInstance()->time;
}

float Engine::DeltaTime()
{
    return GetInstance()->deltaTime;
}

glm::vec2 Engine::GetScreenSize()
{
    return glm::vec2(GetInstance()->GetContext()->width(), GetInstance()->GetContext()->height());
}

glm::vec2 Engine::GetCurrentMousePosition()
{
    QPoint p = Engine::GetInstance()->GetContext()->mapFromGlobal(QCursor::pos());
    return glm::vec2(p.x(), p.y());
}

void Engine::LockCursor(bool lockCursor)
{
    GetInstance()->lockCursor = lockCursor;
}

bool Engine::IsCursorLocked()
{
    return GetInstance()->lockCursor;
}

void Engine::LogVerbose(const std::string &str)
{
    GetInstance()->log->LogLine(Log::Verbose, str);
}

void Engine::LogDebug(const std::string &str)
{
    GetInstance()->log->LogLine(Log::Debug, str);
}

void Engine::LogInfo(const std::string& str)
{
    GetInstance()->log->LogLine(Log::Info, str);
}

void Engine::LogWarning(const std::string &str)
{
    GetInstance()->log->LogLine(Log::Warning, str);
}

void Engine::LogError(const std::string &str)
{
    GetInstance()->log->LogLine(Log::Error, str);
}
