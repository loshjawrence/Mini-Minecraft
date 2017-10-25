#include "input.h"
#include "engine.h"
#include "gameobject.h"
#include <iostream>
#include <typeinfo>

Input::Input() : listeners()
{
}

void Input::RegisterListener(InputListener *l)
{
    this->listeners.push_back(l);

    Component * c = dynamic_cast<Component*>(l);

    if(c != nullptr)
        Engine::LogVerbose("Registering a listener: " + c->ToString());
    else
        Engine::LogVerbose("Registering a listener...");
}

void Input::DeregisterListener(InputListener *l)
{
    InputListenerIterator i = std::find(this->listeners.begin(), this->listeners.end(), l);

    if(i != this->listeners.end())
        this->listeners.erase(i);

    Component * c = dynamic_cast<Component*>(l);

    if(c != nullptr)
        Engine::LogVerbose("Deregistering a listener: " + c->ToString());
    else
        Engine::LogVerbose("Deregistering a listener...");
}

void Input::DispatchKeyPressEvent(QKeyEvent *e)
{
    if(e->isAutoRepeat())
        return;

//    Engine::LogVerbose("Dispatching key press event...");
    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnKeyPressEvent(e);
}

void Input::DispatchKeyReleaseEvent(QKeyEvent *e)
{
    if(e->isAutoRepeat())
        return;

//    Engine::LogVerbose("Dispatching key release event... ");
    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnKeyReleaseEvent(e);
}

void Input::DispatchMousePressEvent(QMouseEvent *e)
{
//    Engine::LogVerbose("Dispatching mouse press event... ");

    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnMousePressEvent(e);
}

void Input::DispatchMouseReleaseEvent(QMouseEvent *e)
{
//    Engine::LogVerbose("Dispatching mouse release event... ");

    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnMouseReleaseEvent(e);
}

void Input::DispatchMouseMoveEvent(QMouseEvent *e)
{
    // Too much frequency
//    Engine::LogDebug("Dispatching mouse move event... ");

    if(e->type() == QMouseEvent::Enter)
        return;

    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnMouseMoveEvent(e);
}

void Input::DispatchMouseWheelEvent(QWheelEvent *e)
{
//    Engine::LogVerbose("Dispatching mouse wheel event... ");

    for(InputListenerIterator i = listeners.begin(); i != listeners.end(); i++)
        (*i)->OnMouseWheelEvent(e);
}
