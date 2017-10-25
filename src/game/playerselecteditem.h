#ifndef PLAYERSELECTEDITEM_H
#define PLAYERSELECTEDITEM_H

#include "player.h"
#include "itemview.h"

class PlayerSelectedItem : public Component
{
private:
    ItemView * currentItemView;

    float timer;
    float momentum;
    bool animating;

public:
    void Awake();
    void Update();

    void StartAnimation();
    void AddMomentum(float direction);
    void SelectItem(Item item);
};

#endif // PLAYERSELECTEDITEM_H
