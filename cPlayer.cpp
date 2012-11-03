#include "cPlayer.h"

extern int posx;
extern int posy;

cPlayer::cPlayer(const char* filename,int x,int y,int w,int h,int xvel,int yvel,int HP)
{
    image = SDL_LoadBMP(filename);
    box.x = x ;
    box.y = y ;
    box.w = w;
    box.h = h;
    xVel = xvel;
    yVel = yvel;
    hp = HP;
    scale = 1;
}

cPlayer::~cPlayer()
{
    SDL_FreeSurface(image);
}

void cPlayer::Movement(int *move)
{
    if(move[0])
    box.y -= yVel;
    if(move[1])
    box.x -= xVel;
    if(move[2])
    box.y += yVel;
    if(move[3])
    box.x += xVel;
}

void cPlayer::Show(int mouseX,int mouseY)
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

SDL_Rect *cPlayer::getRect()
{
    return &box;
}

void cPlayer::SetCamera()
{
    //Center the camera over the dot
    camera.x = ( box.x + box.w / 2 ) - ScreenWidht / 2;
    camera.y = ( box.y + box.h / 2 ) - ScreenHeight / 2;

    //Keep the camera in bounds.
    if( camera.x < 0 )
    {
        camera.x = 0;
    }
    if( camera.y < 0 )
    {
        camera.y = 0;
    }
    if( camera.x > 2000 - camera.w )
    {
        camera.x = 2000 - camera.w;
    }
    if( camera.y > 2000 - camera.h )
    {
        camera.y = 2000 - camera.h;
    }
}

SDL_Rect *cPlayer::getCamera()
{
    return &camera;
}

void cPlayer::PlayerCollision(SDL_Rect *solid,int type,int *move)
{
    if(type == 1)
    {
        if(move[0])
        if(Collision(&box,solid))
        box.y += yVel;
        if(move[1])
        if(Collision(&box,solid))
        box.x += xVel;
        if(move[2])
        if(Collision(&box,solid))
        box.y -= yVel;
        if(move[3])
        if(Collision(&box,solid))
        box.x -= xVel;
    }
    if(type == 2)
    {
        if(move[0])
        if(Collision(&box,solid))
        solid->y -= yVel;
        if(move[1])
        if(Collision(&box,solid))
        solid->x -= xVel;
        if(move[2])
        if(Collision(&box,solid))
        solid->y += yVel;
        if(move[3])
        if(Collision(&box,solid))
        solid->x += xVel;
    }
}
