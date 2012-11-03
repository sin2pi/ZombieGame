#include "cEnemy.h"

cEnemy::cEnemy(const char *filename,int x,int y,int w,int h,int xvel,int yvel,int HP)
{
    image = SDL_LoadBMP(filename);
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;
    xVel = xvel;
    yVel = yvel;
    hp = HP;
    setX = x;
    setY = y;
    scale = 1;
}

cEnemy::~cEnemy()
{
    SDL_FreeSurface(image);
}

SDL_Rect *cEnemy::getRect()
{
    return &box;
}

void cEnemy::SetBack()
{
    box.x = setX;
    box.y = setY;
}

void cEnemy::Show(int mouseX,int mouseY)
{
    float angle = atan2(mouseX - box.x,mouseY - box.y);
    float angle1 = angle * 180 / 3.14159265;
    SDL_SetColorKey(image,SDL_SRCCOLORKEY,SDL_MapRGB(image->format,0,255,255));
    rotated = rotozoomSurface(image,angle1,scale,SMOOTHING_ON);
    SDL_Surface *roto2 = SDL_DisplayFormat(rotated);
    SDL_SetColorKey(roto2,SDL_SRCCOLORKEY,SDL_MapRGB(roto2->format,0,255,255));
    SDL_BlitSurface(roto2,NULL,SDL_GetVideoSurface(),&box);
    SDL_FreeSurface(rotated);
    SDL_FreeSurface(roto2);
}

void cEnemy::Move(int y,int x,SDL_Rect *player)
{
    int e;
    int b;
    if(box.y != y)
    e = 1;
    if(box.x != x)
    b = 1;
    if(e == 1)
     {
          if (y  < box.y){
             box.y -= yVel;
             }
          if (y > box.y){
             box.y += yVel;
             }
     }
     if(box.y == y)
     {
       e = 2;
     }

      if(b == 1)
     {
          if (x  < box.x){
             box.x -= xVel;
             }
          if (x > box.x){
             box.x += xVel;
             }
     }
     if(box.x == x)
     {
       b = 2;
     }
}
