#ifndef CENEMY_H_INCLUDED
#define CENEMY_H_INCLUDED

#include <SDL.h>
#include <math.h>
#include <SDL_rotozoom.h>

class cEnemy
{
    protected:

           int setX, setY;

           SDL_Rect box;

           SDL_Surface *image,*rotated;

    public:

           int hp,xVel,yVel,scale;

           cEnemy(const char *filename,int x,int y,int w,int h,int xvel,int yvel,int HP);

           ~cEnemy();

           SDL_Rect *getRect();

           int* getVel(int xy);

           void SetBack();

           void Move(int y,int x,SDL_Rect *player);

           void Show(int mouseX,int mouseY);
};

#endif // CENEMY_H_INCLUDED
