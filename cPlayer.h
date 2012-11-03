#ifndef CPLAYER_H_INCLUDED
#define CPLAYER_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL_rotozoom.h>
#include <lua.hpp>
#include "Globals.h"
#include "cCollision.h"

class cPlayer : public cCollision
{
    protected:

        SDL_Rect box;

        SDL_Surface *image,*rotated;

        SDL_Rect camera;

    public:

        int hp,xVel,yVel,scale;

        cPlayer(const char *filename,int x,int y,int w,int h,int xvel,int yvel,int HP);

        ~cPlayer();

        void Movement(int *move);

        void Show(int mousex,int mousey);

        void SetCamera();

        void PlayerCollision(SDL_Rect *solid,int type,int *move);

        int ChangeHealth(lua_State *L);

        SDL_Rect *getRect();

        SDL_Rect *getCamera();


};

#endif
