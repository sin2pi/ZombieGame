#include "cBullet.h"

cBullet::cBullet(int curx,int cury,int px,int py,float speed,int str,SDL_Surface* i)
{
	float vecx=curx-px;	//make a vector which points to the cursor
	float vecy=cury-py;
	float len=sqrt(vecx*vecx+vecy*vecy);	//normalize it
	if(len>0)
	{
		vecx/=len;
		vecy/=len;
	}
	vecx *= speed;	//make the speed
	vecy *= speed;

	speedx = vecx;
	speedy = vecy;

	posx = px;
	posy = py;
	strength = str;
	img = i;
}

int cBullet::update(const std::vector<SDL_Rect*>& objects)
{
	posx+=speedx;
	posy+=speedy;
	cCollision col;
	//if you implement scrolling, you should subtract the cam position from position, before do the comparition (see scrolling background tutorial
	//or sidescroller game tutorial)
	if(posx>SDL_GetVideoSurface()->w || posy>SDL_GetVideoSurface()->h || posx+img->w<0 || posy+img->h<0)
		return -2;	//out of screen
	SDL_Rect rec={posx,posy,img->w,img->h};
	for(int i=0;i<objects.size();i++)
		if(col.Collision(objects[i],&rec))
			return 3;	//the bullet hit the ith object (like enemy)
	return -1;	//hit noone
}

void cBullet::show()
{
	SDL_Rect rec={posx,posy};
	SDL_BlitSurface(img,NULL,SDL_GetVideoSurface(),&rec);
}

int cBullet::getStrength()
{
	return strength;
}
