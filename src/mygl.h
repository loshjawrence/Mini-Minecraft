#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include "engine/engine.h"
#include <engine/assets/texture.h>
#include "mainwindow.h"
#include <sstream>
#include <QTextBrowser>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "engine/components/shadow.h"
#include "engine/components/postprocess.h"
#include "engine/components/directionallight.h"
#include "game/weathermanager.h"

class MyGL : public OpenGLContext, public LogListener
{
private:
    GLuint vao;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;
    Engine * engine;
    MainWindow * mainWindow;
    int currentFrame;

    int64_t msec;
    float currentTime;
    float lastUpdateTime;
    bool mouseForceMove;

    std::stringstream consoleOutput;
    int currentConsoleScrollValue;
    Log * logger;

    bool prevCursorLocked;
    void MoveMouseToCenter();

    Shadow* pShadow;
    DirectionalLight* Sun;
    GameObject * pPlayer;
    WeatherManager * weather;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void SetMainWindow(MainWindow * window);

    void OnLogFlush();

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent * e);
    void wheelEvent(QWheelEvent *e);

    void enterEvent(QEvent * e);

private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};

#endif // MYGL_H
