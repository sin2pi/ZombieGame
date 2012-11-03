#include "cCollision.h"

cCollision::cCollision(){}

bool cCollision::Collision(const SDL_Rect *rect1,const SDL_Rect *rect2)
{
    if(rect1->y >= rect2->y + rect2->h)
    return false;
    if(rect1->y + rect1->h <= rect2->y)
    return false;
    if(rect1->x >= rect2->x + rect2->w)
    return false;
    if(rect1->x + rect1->w <= rect2->x)
    return false;
    return true;
}
