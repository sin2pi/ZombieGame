#ifndef CBULLET_H
#define CBULLET_H
#include <SDL/SDL.h>
#include <vector>
#include <cmath>
#include "cCollision.h"

class cBullet{
	int speedx,speedy;
	float posx,posy;
	int strength;
	SDL_Surface* img;
	public:
		cBullet(int curx,int cury,int px,int py,float speed,int str,SDL_Surface* i);
		int update(const std::vector<SDL_Rect*>& objects);
		void show();
		int getStrength();
};
#endif
