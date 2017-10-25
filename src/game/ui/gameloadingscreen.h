#ifndef GAMELOADINGSCREEN_H
#define GAMELOADINGSCREEN_H

#include "../../engine/common.h"

class GameLoadingScreen : public UIComponent
{
protected:
    UIImage * logo;
    UIImage * background;

    float timer;

public:
    void Start();
    void Update();
};

#endif // GAMELOADINGSCREEN_H
