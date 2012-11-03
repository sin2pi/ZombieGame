#ifndef CCOLLISION_H_INCLUDED
#define CCOLLISION_H_INCLUDED

#include <SDL/SDL.h>

class cCollision
{
    public:

           cCollision();

           bool Collision(const SDL_Rect *rect1,const SDL_Rect *rect2);

};

#endif // CCOLLISION_H_INCLUDED
