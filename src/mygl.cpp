#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QDateTime>
#include "engine/common.h"
#include "game/common.h"

#include "game/ui/inventorytoolbox.h"
#include "game/ui/uiinventoryscreen.h"
#include "game/playercross.h"
#include "game/chunkmanager.h"

#include "game/playerphysics.h"
#include "game/procedural/lsystem.h"
#include "game/ui/gameloadingscreen.h"
#include <QScrollBar>

#include "engine/components/skybox.h"

#define DEBUG_ENGINE false

void MyGL::MoveMouseToCenter()
{
    mouseForceMove = true;
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent), engine(Engine::GetInstance()), msec(0), currentTime(0.f), lastUpdateTime(0.f), mouseForceMove(false), mainWindow(nullptr), currentFrame(0),
      consoleOutput(), currentConsoleScrollValue(0)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

    // Tell the timer to redraw 100 times per second, even if they queue
    timer.start(10);
    setFocusPolicy(Qt::ClickFocus);

    engine->Initialize(this);

    this->logger = new RichTextLog(&consoleOutput);
    engine->AddLogger(logger);

    engine->GetLog()->SetLogLevel(Log::Debug);
    engine->Test();

    this->setMouseTracking(true);
    this->setCursor(Qt::BlankCursor);
    this->MoveMouseToCenter();
    prevCursorLocked = true;

    pPlayer = nullptr;
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();


    // Set a few settings/mods in OpenGL rendering

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Set the size with which points should be rendered
    glPointSize(5);
	//glPolygonOffset(1.1, 4.4);

    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    // vao.bind();
    glBindVertexArray(vao);

    // Careful about this: if we set mygl as listener before Qt is initialized, the scrollbar explodes
    logger->SetListener(this);

    GameObject * weathersystem = GameObject::Instantiate(std::string("WeatherSystem"));
    WeatherSystem* weathersystem_handle = weathersystem->AddComponent<WeatherSystem>();

    GameObject * skybox = GameObject::Instantiate(std::string("SkyBox"));
    skybox->AddComponent<SkyBox>();

    //SUN
    GameObject * directionallight = GameObject::Instantiate(std::string("DirectionalLight"));
    Sun = directionallight->AddComponent<DirectionalLight>();

    // Just to debug engine and not the game itself
    if(!DEBUG_ENGINE)
    {
        GameObject * playerGameObject = GameObject::Instantiate(std::string("Player"));
        Player * player = playerGameObject->AddComponent<Player>();
        pPlayer = playerGameObject;

        GameObject * chunkman = GameObject::Instantiate(std::string("ChunkManager"));
        ChunkManager * chunk = chunkman->AddComponent<ChunkManager>();
        chunk->player = player;

        GameObject * weatherman = GameObject::Instantiate(std::string("WeatherManager"));
        weather = weatherman->AddComponent<WeatherManager>();
        weather->init(chunk,player,weathersystem_handle,Sun);

        playerGameObject->GetComponent<PlayerPhysics>()->chunkManager = chunk;
        playerGameObject->GetTransform()->SetWorldPosition(glm::vec3(0, CHUNK_HEIGHT * 5.f / 8.f, 0));
        playerGameObject->GetComponent<PlayerInput>()->EnableFly(true);

        UIInventoryToolbox * toolbox = GameObject::Instantiate(std::string("InventoryToolbox"))->AddComponent<UIInventoryToolbox>();
        toolbox->SetPlayer(player);

        UIInventoryScreen * inventoryScreen = GameObject::Instantiate("InventoryScreen")->AddComponent<UIInventoryScreen>();
        inventoryScreen->GetGameObject()->SetEnabled(false);
        inventoryScreen->SetPlayer(player);

        GameObject::Instantiate("LoadingScreen")->AddComponent<GameLoadingScreen>();
    }

    msec = QDateTime::currentMSecsSinceEpoch();

	GameObject * shadow = GameObject::Instantiate(std::string("Shadow"));

	pShadow = shadow->AddComponent<Shadow>();
	pShadow->Initialize(this, ShadowMapCameraSize01, ShadowMapCameraSize01,
		ShadowMapCameraSize02, ShadowMapCameraSize02,
		ShadowMapCameraSize03, ShadowMapCameraSize03,
        Sun /*&(engine->m_DirectionalLight)*/);

	for (uint i = 0; i < SHADOWMAPCOUNT; i++)
	{
		engine->m_ShadowFrameBuffer[i] = pShadow->m_ShadowMap[i].m_Framebuffer;
		engine->m_ShadowRenderBuffer[i] = pShadow->m_ShadowMap[i].m_Renderbuffer;
		engine->m_ShadowMapResource[i] = &pShadow->m_ShadowMap[i];
    }

    GameObject * postprocess = GameObject::Instantiate(std::string("PostProcess"));
    PostProcess * pp = postprocess->AddComponent<PostProcess>();
    pp->Initialize(engine->GetMainCamera(), "snow");

    // Hide console by default
    this->mainWindow->GetConsoleLabel()->setVisible(false);
}

// This method is called by Qt when widget size changes
void MyGL::resizeGL(int w, int h)
{
    this->engine->OnOpenGLContextChanged();
}

// This method is called by Qt
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float deltaTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;


    engine->m_DirectionalLightDir = Sun->m_direction;
    engine->m_DirectionalLightCol = Sun->m_color;
    engine->m_DirectionalLightDaytime = Sun->m_daytime;

    if(pPlayer)
        pShadow->UpdateInfo(pPlayer->GetTransform()->WorldPosition());

	for (uint i = 0; i < SHADOWMAPCOUNT; i++)
	{
		engine->shadowmapVP[i] = pShadow->shadowmapVP[i];
	}



    engine->Update(deltaTime);

    mainWindow->GetConsoleLabel()->verticalScrollBar()->setValue(currentConsoleScrollValue);
    mainWindow->GetConsoleLabel()->resize(mainWindow->width(), mainWindow->height() / 5);
    mainWindow->GetConsoleLabel()->move(0, (mainWindow->height() / 5) * 4);
}

void MyGL::SetMainWindow(MainWindow *window)
{
    this->mainWindow = window;
}

void MyGL::OnLogFlush()
{
    mainWindow->GetConsoleLabel()->setText(QString::fromStdString(consoleOutput.str()));
    currentConsoleScrollValue = mainWindow->GetConsoleLabel()->verticalScrollBar()->maximum();
}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    this->engine->GetInput()->DispatchKeyReleaseEvent(e);

    if(e->key() == Qt::Key_Tab)
        this->mainWindow->GetConsoleLabel()->setVisible(!this->mainWindow->GetConsoleLabel()->isVisible());
}

void MyGL::mousePressEvent(QMouseEvent *e)
{
    this->engine->GetInput()->DispatchMousePressEvent(e);
}

void MyGL::mouseReleaseEvent(QMouseEvent *e)
{
    this->engine->GetInput()->DispatchMouseReleaseEvent(e);
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    if(!mouseForceMove)
    {
        // Dont dispatch frames where there are huge mouse changes
        if(engine->IsCursorLocked() == prevCursorLocked)
            this->engine->GetInput()->DispatchMouseMoveEvent(e);

        if(engine->IsCursorLocked())
        {
            this->setCursor(Qt::BlankCursor);
            MoveMouseToCenter();
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
        }

        prevCursorLocked = engine->IsCursorLocked();
    }

    mouseForceMove = false;
}

void MyGL::wheelEvent(QWheelEvent *e)
{
    currentConsoleScrollValue += e->delta() > 0 ? -35 : 35;
    currentConsoleScrollValue = glm::clamp(currentConsoleScrollValue, mainWindow->GetConsoleLabel()->verticalScrollBar()->minimum(), mainWindow->GetConsoleLabel()->verticalScrollBar()->maximum());

    this->engine->GetInput()->DispatchMouseWheelEvent(e);
}

void MyGL::enterEvent(QEvent *e)
{
    MoveMouseToCenter();
}

// Method called by Qt event system
void MyGL::keyPressEvent(QKeyEvent *e)
{
    this->engine->GetInput()->DispatchKeyPressEvent(e);

    // This overrides everything
    if (e->key() == Qt::Key_Escape)
            QApplication::quit();

    if(e->key() == Qt::Key_I)
        Sun->Faster();

    if(e->key() == Qt::Key_O)
        Sun->Slower();

    if(e->key() == Qt::Key_P)
        Sun->Pause();

    if(e->key() == Qt::Key_X)
        this->engine->DebugShade();

    if(e->key() == Qt::Key_Z)
        weather->ToggleWeahter();


}

// Called by timer
void MyGL::timerUpdate()
{
    // Timers lie, so we need the real time delta...
    int64_t m = QDateTime::currentMSecsSinceEpoch();
    int64_t delta = m - msec;

    currentFrame++;

    this->currentTime += delta / 1000.f;
    this->update();

    if(mainWindow != nullptr && (currentFrame % 10) == 0  && delta != 0)
        this->mainWindow->SetFPS(1000.f / delta);

    msec = m;
}
