#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <QKeyEvent>
#include <QWheelEvent>

// Good old-fashioned observer pattern
class InputListener
{
public:
    // These are the only ones we need now...
    virtual void OnKeyPressEvent(QKeyEvent * e){}
    virtual void OnKeyReleaseEvent(QKeyEvent * e){}

    virtual void OnMousePressEvent(QMouseEvent * e){}
    virtual void OnMouseReleaseEvent(QMouseEvent * e){}
    virtual void OnMouseMoveEvent(QMouseEvent * e){}
    virtual void OnMouseWheelEvent(QWheelEvent * e){}

    virtual ~InputListener(){}
};

// Input handler for our engine
// For now, is uses Qt's input primitives, but in the future it should be abstracted...
class Input
{
protected:
    std::vector<InputListener*> listeners;
    typedef std::vector<InputListener*>::iterator InputListenerIterator;

public:
    Input();

    void RegisterListener(InputListener * l);
    void DeregisterListener(InputListener * l);

    void DispatchKeyPressEvent(QKeyEvent * e);
    void DispatchKeyReleaseEvent(QKeyEvent * e);

    void DispatchMousePressEvent(QMouseEvent * e);
    void DispatchMouseReleaseEvent(QMouseEvent * e);
    void DispatchMouseMoveEvent(QMouseEvent * e);
    void DispatchMouseWheelEvent(QWheelEvent * e);
};

#endif // INPUT_H
