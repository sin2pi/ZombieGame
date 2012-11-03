/*
  Copyright HoneyComb Studios 2012 - 2013
  All rights reserved
  This work is property of HoneyComb Studios,
  any reproduction, publish or public use of
  this work without previous permission
  is strictly forbidden, you have been warned.
        -Martin Dionisi, CEO HoneyComb Studios
*/
extern "C"
{
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL_rotozoom.h>
#include <SDL_mixer.h>
#include <vector>
#include <lua.hpp>
#include <fstream>
#include <math.h>
#include "Globals.h"
#include "cPlayer.h"
#include "cCollision.h"
#include "cBullet.h"
#include "cEnemy.h"
#include "Lua_Functions.h"
#include "LuaPrompt.h"

using namespace std;

SDL_Surface *screen;
SDL_Surface *rotated;
SDL_Surface *menuBack;
SDL_Surface *cursor;

int move[4] = {0,0,0,0};

//Shit variables
int score;
int maxScore;
int type = 1;
int lives = 3;
int Gamestate = 1;
int pause = 0;
int bulletsShooted = 0;
int bombsShooted = 0;
int bulletHit = 0;
int BombAmount = 5;
int BulletAmount = 200;
int enem1Hp = 10;
int enem2Hp = 50;
int enem3Hp = 100;
int enem11Hp = 10;

Uint32 start;
int FPS = 60;

// Save Load stats
bool saveStats;
bool loadStats;
bool setBack;
bool PrompActive = false;

SDL_Color Green = {0,255,0};
SDL_Event event;
TTF_Font *font2;
TTF_Font *font;
TTF_Font *fontMenu;

//Self explanatiory
bool running = true;

LuaPrompt prompt;

cPlayer player1("Images/player.bmp",320,300,50,50,3,3,500);
cEnemy enemy1("Images/enemy.bmp",1000,200,50,50,2,2,10);
cEnemy enemy11("Images/enemy.bmp",300,1000,50,50,2,2,10);
cEnemy enemy2("Images/enemy2.bmp",1000,200,50,50,2,2,50);
cEnemy enemy3("Images/enemy3.bmp",200,2000,50,50,2,2,100);

void BlitSurface(SDL_Surface *image,SDL_Surface *screen,int x,int y)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_BlitSurface(image,NULL,screen,&rect);
}

void Init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
    screen = SDL_SetVideoMode(ScreenWidht,ScreenHeight,Bps,SDL_SWSURFACE);
    SDL_WM_SetCaption("Zombie Massacre",NULL);
}

lua_State *L = lua_open();

int main(int argc,char *argv[])
{
    Init();
    //All lua shit!
    luaL_openlibs(L);
    //registering lua functions!
    lua_register(L,"PlayerHealth",Player_SetHealth);
    lua_register(L,"PlayerPosX",Player_SetPosX);
    lua_register(L,"PlayerPosY",Player_SetPosY);
    lua_register(L,"EnemyPosX",Enemy_SetPosX);
    lua_register(L,"EnemyPosY",Enemy_SetPosY);
    lua_register(L,"EnemyHealth",Enemy_SetHealth);
    lua_register(L,"BulletAmount",Player_Bullet);
    lua_register(L,"BombAmount",Player_Bomb);
    lua_register(L,"PlayerXvel",Player_xVel);
    lua_register(L,"PlayerYvel",Player_yVel);
    lua_register(L,"EnemyXvel",Enemy_xVel);
    lua_register(L,"EnemyYvel",Enemy_yVel);
    lua_register(L,"SetAI",SetAI);
    lua_register(L,"PlayerScale",Player_Scale);
    lua_register(L,"EnemyScale",Enemy_Scale);
    lua_register(L,"LoadScript",Load_Script);
    lua_register(L,"LoadMusic",LoadMusic);
    lua_register(L,"PlayerLives",Player_lives);

    std::vector<cBullet> bombs;
    std::vector<cBullet> bullets;	//all the bullets
    std::vector<SDL_Rect*> enemVec3;
    std::vector<SDL_Rect*> enemVec2;
    std::vector<SDL_Rect*> enemVec1;
    std::vector<SDL_Rect*> enemVec11; //add enemy rects (or rects, which should stop a bullet) at the end of this vector
    //if you later create a class for the enemies, you should make the type that (but change in the bullet update function too
    //you can make different for enemies and tiles, which the bullet cannot go through
    //colors
    SDL_Color color = {0,0,0};
    SDL_Color color1 = {0,0,0};
    SDL_Color color2 = {0,0,0};
    SDL_Color color3 = {0,0,0};
    SDL_Color color4 = {0,0,0};
    SDL_Color color5 = {0,0,0};
    SDL_Color color6 = {0,0,0};
    SDL_Color color10 = {0,0,0};
    SDL_Color color11 = {0,0,0};
    SDL_Color color12 = {0,0,0};

    SDL_ShowCursor(0);
    SDL_Rect cursorRect;

    //load shit
    TTF_Font *fontMin = TTF_OpenFont("Fonts/arial.ttf",15);
    font = TTF_OpenFont("Fonts/dalila.ttf",30);
    font2 = TTF_OpenFont("Fonts/arial.ttf",20);
    fontMenu = TTF_OpenFont("Fonts/dalila.ttf",60);
    menuBack = SDL_LoadBMP("Images/menu.bmp");
    SDL_Surface* bulletimage = SDL_LoadBMP("Images/bullet.bmp");
    SDL_Surface* bombimage = SDL_LoadBMP("Images/bomb.bmp");
    cursor = SDL_LoadBMP("Images/CURSOR.bmp");
    SDL_SetColorKey(cursor,SDL_SRCCOLORKEY,SDL_MapRGB(cursor->format,0,255,255));

    int posx,posy;

    //Chucks & music
    Mix_Chunk *walking;
    Mix_Chunk *shoot;
    Mix_Chunk *empty;
    Mix_Chunk *granade;
    Mix_Chunk *zombie;

    granade = Mix_LoadWAV("Music/shoot.wav");
    walking = Mix_LoadWAV("Music/walking.wav");
    shoot = Mix_LoadWAV("Music/granade.wav");
    empty = Mix_LoadWAV("Music/empty.wav");

    //Menu
    SDL_Rect startGame;
    SDL_Rect stats;
    SDL_Rect help;
    SDL_Rect difficult;
    SDL_Rect Bmb;
    SDL_Rect MaxScore;
    SDL_Rect hpRect;
    SDL_Rect scoreRect;
    SDL_Rect maxRect;
    SDL_Rect bulletRect;
    SDL_Rect bombRect;
    SDL_Rect liveRect;
    SDL_Rect resume;
    SDL_Rect exit;
    SDL_Rect Bull;
    SDL_Rect Acc;
    SDL_Rect medium1;
    SDL_Rect hard1;
    SDL_Rect easy1;
    SDL_Rect fpsRect = {20,50};
    SDL_Rect useless = {0,0,0,0};

    SDL_Surface *startGameSur;
    SDL_Surface *statsSur;
    SDL_Surface *helpSur;
    SDL_Surface *difficultSur;
    SDL_Surface *textHard1;
    SDL_Surface *textMedium1;
    SDL_Surface *textEasy1;

    startGame.x = ScreenWidht/2 - 150;
    startGame.y = ScreenHeight/2 - 120;

    stats.x = ScreenWidht/2 - 150;
    stats.y = ScreenHeight/2 - 50;

    help.x = ScreenWidht/2 - 150;
    help.y = ScreenHeight/2 + 20;

    difficult.x = ScreenWidht/2 - 150;
    difficult.y = ScreenHeight/2 + 90;

    SDL_Surface *fpsSur;

    ///Text rects
    //In game rects

    hpRect.x = 20;
    hpRect.y = 10;
    maxRect.x = ScreenWidht/2 - 110;
    maxRect.y = 10;
    scoreRect.x = 490;
    scoreRect.y = 10;
    bulletRect.x = 20;
    bulletRect.y = 430;
    bombRect.x = 490;
    bombRect.y = 430;
    liveRect.x = ScreenWidht/2 - 40;
    liveRect.y = 430;
    resume.x = ScreenWidht/2 - 90;
    resume.y = ScreenHeight/2 - 100;
    exit.x = ScreenWidht/2 - 90;
    exit.y = ScreenHeight/2 - 30;
    MaxScore.x = 640/2 - 150;
    MaxScore.y = 480/2 - 120;
    Bull.x = ScreenWidht/2 - 150;
    Bull.y = ScreenHeight/2 + 20;
    Bmb.x = ScreenWidht/2 - 150;
    Bmb.y = ScreenHeight/2 - 50;
    Acc.x = ScreenWidht/2 - 150;
    Acc.y = ScreenHeight/2 + 90;
    hard1.x = ScreenWidht/2 - 100;
    hard1.y = ScreenHeight/2 - 100;
    medium1.x = ScreenWidht/2 - 100;
    medium1.y = ScreenHeight/2 - 30;
    easy1.x = ScreenWidht/2 - 100;
    easy1.y = ScreenHeight/2 + 40;

    SDL_SetColorKey(bulletimage,SDL_SRCCOLORKEY,SDL_MapRGB(bulletimage->format,0,255,255));
    SDL_SetColorKey(bombimage,SDL_SRCCOLORKEY,SDL_MapRGB(bombimage->format,0,255,255));
    int x,y;
    y = 0;
    x = 0;
    fstream openFile;
    openFile.open("Saves/score.txt");
    openFile >> maxScore;
    openFile >> bulletsShooted;
    openFile >> bombsShooted;
    openFile >> bulletHit;
    // Background
    SDL_Surface *background;
    background = SDL_LoadBMP("Images/background.bmp");
    cCollision col;
    while(running)
    {
       start = SDL_GetTicks();
       while (SDL_PollEvent(&event))
       {
         switch(event.type)
          {
            case SDL_QUIT:
                 running = false;
                 break;
            case SDL_KEYDOWN:
            if(PrompActive == true && Gamestate == 2 && pause == 3)
            {
               prompt.PromptActive = true;
               prompt.handle_input();
            }
              switch(event.key.keysym.sym)
                {
                  case SDLK_ESCAPE:
                       if(Gamestate == 2)
                       pause = 1;
                       if(Gamestate == 3)
                       Gamestate = 1;
                       if(Gamestate == 4)
                       Gamestate = 1;
                       break;
                  case SDLK_s:
                       if(Gamestate == 2 && pause == 0){
                       loadStats = true;
                       }
                       break;
                  case SDLK_SPACE:
                       if(Gamestate == 2 && pause == 0){
                       saveStats = true;
                       }
                       break;
                  case SDLK_1:
                       if(Gamestate == 2)
                       {

                       }
                       break;
                 case SDLK_BACKQUOTE:
                      if(PrompActive == false)
                      {
                         PrompActive = true;
                         pause = 3;
                      }
                      else
                      {
                         PrompActive = false;
                         pause = NULL;
                      }
                 break;
                  case SDLK_UP:
                       if(Gamestate == 2 && pause == 0){
                       Mix_PlayChannel(1,walking,1);
                       move[0] = 1;
                       }
                       break;
                  case SDLK_RIGHT:
                       if(Gamestate == 2 && pause == 0){
                       Mix_PlayChannel(1,walking,1);
                       move[3] = 1;
                       }
                       break;
                  case SDLK_DOWN:
                       if(Gamestate == 2 && pause == 0){
                       Mix_PlayChannel(1,walking,1);
                       move[2] = 1;
                       }
                       break;
                  case SDLK_LEFT:
                       if(Gamestate == 2 && pause == 0){
                       Mix_PlayChannel(1,walking,1);
                       move[1] = 1;
                       }
                       break;
                  default:
                  break;
                  }
                  break;

            case SDL_KEYUP:

                switch(event.key.keysym.sym)
                {
                  case SDLK_UP:
                       if(Gamestate == 2 && pause == 0){
                       Mix_Pause(1);
                       move[0] = 0;
                       }
                       break;
                  case SDLK_RIGHT:
                       if(Gamestate == 2 && pause == 0){
                       Mix_Pause(1);
                       move[3] = 0;
                       }
                       break;
                  case SDLK_DOWN:
                       if(Gamestate == 2 && pause == 0){
                       Mix_Pause(1);
                       move[2] = 0;
                       }
                       break;
                  case SDLK_LEFT:
                       if(Gamestate == 2 && pause == 0){
                       Mix_Pause(1);
                       move[1] = 0;
                       }
                       break;
                  default:
                  break;
                }
                break;
            case SDL_MOUSEMOTION:
            posx=event.button.x;
            posy=event.button.y;
            if(Gamestate == 1 && posx > startGame.x && posx < startGame.x + startGame.w && posy > startGame.y && posy < startGame.y + startGame.h)
            {
                color1 = {255,0,0};
            }
            else
            {
                color1 = {0,0,0};
            }
            if(Gamestate == 1 && posx > stats.x && posx < stats.x + stats.w && posy > stats.y && posy < stats.y + stats.h)
            {
                color2 = {255,0,0};
            }
            else
            {
                color2 = {0,0,0};
            }
            if(Gamestate == 1 && posx > help.x && posx < help.x + help.w && posy > help.y && posy < help.y + help.h)
            {
                color3 = {255,0,0};
            }
            else
            {
                color3 = {0,0,0};
            }
            if(Gamestate == 1 && posx > difficult.x && posx < difficult.x + difficult.w && posy > difficult.y && posy < difficult.y + difficult.h)
            {
                color4 = {255,0,0};
            }
            else
            {
                color4 = {0,0,0};
            }
            if(Gamestate == 5 && posx > easy1.x && posx < easy1.x + easy1.w && posy > easy1.y && posy < easy1.y + easy1.h)
            {
                color10 = {255,0,0};
            }
            else
            {
                color10 = {0,0,0};
            }
            if(Gamestate == 5 && posx > medium1.x && posx < medium1.x + medium1.w && posy > medium1.y && posy < medium1.y + medium1.h)
            {
                color11 = {255,0,0};
            }
            else
            {
                color11 = {0,0,0};
            }
            if(Gamestate == 5 && posx > hard1.x && posx < hard1.x + hard1.w && posy > hard1.y && posy < hard1.y + hard1.h)
            {
                color12 = {255,0,0};
            }
            else
            {
                color12 = {0,0,0};
            }
            if(Gamestate == 2 && pause == 1 && posx > resume.x && posx < resume.x + resume.w && posy > resume.y && posy < resume.y + resume.h)
            {
                color5 = {255,0,0};
            }
            else
            {
                color5 = {0,0,0};
            }
            if(Gamestate == 2 && pause == 1 && posx > exit.x && posx < exit.x + exit.w && posy > exit.y && posy < exit.y + exit.h)
            {
                color6 = {255,0,0};
            }
            else
            {
                color6 = {0,0,0};
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            x=event.button.x;
            y=event.button.y;
            if(Gamestate == 1 && posx > startGame.x && posx < startGame.x + startGame.w && posy > startGame.y && posy < startGame.y + startGame.h)
            {
                Gamestate = 2;
            }
            if(Gamestate == 1 && posx > stats.x && posx < stats.x + stats.w && posy > stats.y && posy < stats.y + stats.h)
            {
                Gamestate = 3;
            }
            if(Gamestate == 1 && posx > difficult.x && posx < difficult.x + difficult.w && posy > difficult.y && posy < difficult.y + difficult.h)
            {
                Gamestate = 5;
            }
            if(Gamestate == 2 && pause == 1 && posx > resume.x && posx < resume.x + resume.w && posy > resume.y && posy < resume.y + resume.h)
            {
                pause = 0;
            }
            if(Gamestate == 2 && pause == 1 && posx > exit.x && posx < exit.x + exit.w && posy > exit.y && posy < exit.y + exit.h)
            {
                Gamestate = 1;
                pause = 0;
                //SetBack
                setBack = true;
            }
            if(Gamestate == 1 && posx > help.x && posx < help.x + help.w && posy > help.y && posy < help.y + help.h)
            {
                Gamestate = 4;
            }
            if(Gamestate == 5 && posx > easy1.x && posx < easy1.x + easy1.w && posy > easy1.y && posy < easy1.y + easy1.h)
            {
                luaL_dofile(L,"Scripts/hard.lua");
                enem1Hp = 15;
                enem2Hp = 70;
                enem3Hp = 130;
                enem11Hp = 15;
                Gamestate = 1;
            }
            if(Gamestate == 5 && posx > medium1.x && posx < medium1.x + medium1.w && posy > medium1.y && posy < medium1.y + medium1.h)
            {
                luaL_dofile(L,"Scripts/medium.lua");
                enem1Hp = 10;
                enem2Hp = 50;
                enem3Hp = 100;
                enem11Hp = 10;
                Gamestate = 1;
            }
            if(Gamestate == 5 && posx > hard1.x && posx < hard1.x + hard1.w && posy > hard1.y && posy < hard1.y + hard1.h)
            {
                luaL_dofile(L,"Scripts/easy.lua");
                enem1Hp = 5;
                enem2Hp = 30;
                enem3Hp = 70;
                enem11Hp = 5;
                Gamestate = 1;
            }
            switch(event.button.button)
            {
                case SDL_BUTTON_LEFT:
                {
                    if(Gamestate == 2 && pause == 0){
                    if(player1.hp > 0)
                    {
                        BulletAmount --;
                        if(BulletAmount >= 0)
                        {
                           Mix_PlayChannel(2,shoot,0);
                           bullets.push_back(cBullet(x,y,player1.getRect()->x + 30,player1.getRect()->y + 30,5,5,bulletimage));
                           bulletsShooted ++;
                        }
                        if(BulletAmount <= 0)
                        {
                            Mix_PlayChannel(3,empty,0);
                        }
                    }
                 }
                 break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    if(Gamestate == 2 && pause == 0){
                    if(player1.hp > 0)
                    {
                        BombAmount --;
                        if(BombAmount >= 0)
                        {
                            bombs.push_back(cBullet(x,y,player1.getRect()->x,player1.getRect()->y,0,0,bombimage));
                            bombsShooted ++;
                        }
                    }
                    }
                    break;
                }
                default:
                break;
            }
           	break;
            default:
            break;
       }
    }

    cursorRect.x = posx;
    cursorRect.y = posy;

    openFile.open("Saves/score.txt");
    openFile << maxScore << " ";
    openFile << bulletsShooted << " ";
    openFile << bombsShooted << " ";
    openFile << bulletHit << " ";
    openFile.close();

    if(score > maxScore)
    {
        maxScore = score;
    }

   //if the game is running
   if(Gamestate == 2 && pause == 0){

    SDL_BlitSurface(background,NULL,screen,NULL);

    //if the player is fuking alive LOL
    if(player1.hp > 0)
    {
        player1.Movement(move);
        for(int i=0;i<bombs.size();i++)
        {
        	int enem1=bombs[i].update(enemVec1);
            int enem2=bombs[i].update(enemVec2);
            int enem3=bombs[i].update(enemVec3);
            int enem11=bombs[i].update(enemVec11);
        	if(enem1 == -2 || enem2 == -2)
        	{
        		//out of the screen
        		bombs.erase(bombs.begin()+i);
        	}else if(enem1 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bombs.erase(bombs.begin()+i);
        		enemy1.hp -= 10;
        		score += 2;
        		Mix_PlayChannel(4,granade,0);
        	}
        	else if(enem2 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bombs.erase(bombs.begin()+i);
        		enemy2.hp -= 10;
        		score += 2;
        		Mix_PlayChannel(4,granade,0);
        	}
            else if(enem3 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bombs.erase(bombs.begin()+i);
        		enemy3.hp -= 10;
        		score += 2;
        		Mix_PlayChannel(4,granade,0);
        	}
            else if(enem11 == 3)
        	{
        		//bullet hit the it object, here you could decrease the enemy's health
        		bombs.erase(bombs.begin()+i);
        		enemy11.hp -= 10;
        		score += 2;
        		Mix_PlayChannel(4,granade,0);
        	}
        	bombs[i].show();
        }
        for(int i=0;i<bullets.size();i++)
        {
        	int enem1=bullets[i].update(enemVec1);
            int enem2=bullets[i].update(enemVec2);
            int enem3=bullets[i].update(enemVec3);
            int enem11=bullets[i].update(enemVec11);
        	if(enem1 == -2 || enem2 == -2)
        	{
        		//out of the screen
        		bullets.erase(bullets.begin()+i);
        	}else if(enem1 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bullets.erase(bullets.begin()+i);
        		enemy1.hp -= 2;
        		score ++;
        		bulletHit ++;
        	}
        	else if(enem2 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bullets.erase(bullets.begin()+i);
        		enemy2.hp -= 2;
        		score ++;
        		bulletHit ++;
        	}
            else if(enem3 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bullets.erase(bullets.begin()+i);
        		enemy3.hp -= 2;
        		score ++;
        		bulletHit ++;
        	}
            else if(enem11 == 3)
        	{
        		//bullet hit the ith object, here you could decrease the enemy's health
        		bullets.erase(bullets.begin()+i);
        		enemy11.hp -= 2;
        		score ++;
        		bulletHit ++;
        	}
        	bullets[i].show();
        }
        player1.Show(posx,posy);
        //luaL_dofile(L,"Scripts/AI.lua");
    }
    if(player1.hp <= 0)
    {
        player1.hp = 0;
        lives --;
        if(lives >= 0)
        {
            luaL_dofile(L,"Scripts/Set_Back.lua");
        }
        if(lives < 0)
        {
            setBack = true;
            Gamestate = 1;
            lives = 3;
        }
    }

    //Fuck, the enemy is alive!
    if(enemy1.hp > 0)
    {
       enemVec1.push_back(enemy1.getRect());
       enemy1.Show(player1.getRect()->x,player1.getRect()->y);
       luaL_dofile(L,"Scripts/AI-1.lua");
    }
    if(enemy1.hp <= 0)
    {
        enemy1.getRect()->x = 1000;
        enemy1.hp = enem1Hp;
        type ++;
        BulletAmount += 6;
    }
    if(enemy2.hp > 0 && type >= 20)
    {
       enemVec2.push_back(enemy2.getRect());
       enemy2.Show(player1.getRect()->x,player1.getRect()->y);
       luaL_dofile(L,"Scripts/AI-2.lua");
    }
    if(enemy2.hp <= 0)
    {
        enemy2.getRect()->y = 1000;
        enemy2.hp = enem2Hp;
        type ++;
        BombAmount += 1;
        BulletAmount += 10;
    }
    if(enemy3.hp > 0 && type >= 30)
    {
       enemVec3.push_back(enemy3.getRect());
       enemy3.Show(player1.getRect()->x,player1.getRect()->y);
       luaL_dofile(L,"Scripts/AI-3.lua");
    }
    if(enemy3.hp <= 0)
    {
        enemy3.getRect()->y = 1000;
        enemy3.hp = enem3Hp;
        type ++;
        BombAmount += 3;
        BulletAmount += 30;
    }
    if(enemy11.hp > 0 && type >= 2)
    {
       enemVec11.push_back(enemy11.getRect());
       enemy11.Show(player1.getRect()->x,player1.getRect()->y);
       luaL_dofile(L,"Scripts/AI-11.lua");
    }
    if(enemy11.hp <= 0)
    {
        enemy11.getRect()->y = 1000;
        enemy11.hp = enem11Hp;
        type ++;
        BombAmount += 1;
        BulletAmount += 6;
    }

    if(col.Collision(player1.getRect(),enemy1.getRect()) || col.Collision(player1.getRect(),enemy2.getRect())|| col.Collision(player1.getRect(),enemy3.getRect())|| col.Collision(player1.getRect(),enemy11.getRect()))
    {
        player1.hp --;
    }

    //Keep the player on bounds
    if(player1.getRect()->x + player1.getRect()->w >= ScreenWidht)
    {
        player1.getRect()->x -= 2;
    }
    if(player1.getRect()->y + player1.getRect()->h >= ScreenHeight)
    {
        player1.getRect()->y -= 2;
    }

    if(BulletAmount <= 0)
    {
        BulletAmount = 0;
    }
    if(BombAmount <= 0)
    {
        BombAmount = 0;
    }

        if(saveStats == true)
        {
            fstream file1;
            file1.open("Saves/save.txt");
            file1 << score << " ";
            file1 << type << " ";
            file1 << player1.hp << " ";
            file1 << lives << " ";
            file1 << player1.getRect()->x << " ";
            file1 << player1.getRect()->y << " ";
            file1 << enemy1.getRect()->x << " ";
            file1 << enemy1.getRect()->y << " ";
            file1 << enemy2.getRect()->x << " ";
            file1 << enemy2.getRect()->y << " ";
            file1 << enemy3.getRect()->x << " ";
            file1 << enemy3.getRect()->y << " ";
            file1 << enemy11.getRect()->x << " ";
            file1 << enemy11.getRect()->y << " ";
            file1 << BulletAmount << " ";
            file1 << BombAmount << " ";
            file1.close();
            saveStats = false;
        }
        if(loadStats == true)
        {
            fstream file2;
            file2.open("Saves/save.txt");
            file2 >> score;
            file2 >> type;
            file2 >> player1.hp;
            file2 >> lives;
            file2 >> player1.getRect()->x;
            file2 >> player1.getRect()->y;
            file2 >> enemy1.getRect()->x;
            file2 >> enemy1.getRect()->y;
            file2 >> enemy2.getRect()->x;
            file2 >> enemy2.getRect()->y;
            file2 >> enemy3.getRect()->x;
            file2 >> enemy3.getRect()->y;
            file2 >> enemy11.getRect()->x;
            file2 >> enemy11.getRect()->y;
            file2 >> BulletAmount;
            file2 >> BombAmount;
            file2 >> bulletsShooted;
            file2 >> bombsShooted;
            file2.close();
            loadStats = false;
        }

        char c[5];
        sprintf(c,"HP: %d",player1.hp);
        SDL_Surface *text,*text2,*text3,*text4,*text5,*text6;
        text = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text,NULL,screen,&hpRect);
        sprintf(c,"Score: %d",score);
        text2 = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text2,NULL,screen,&scoreRect);
        sprintf(c,"Bullets: %d",BulletAmount);
        text3 = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text3,NULL,screen,&bulletRect);
        sprintf(c,"Mines: %d",BombAmount);
        text4 = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text4,NULL,screen,&bombRect);
        sprintf(c,"Max Score: %d",maxScore);
        text5 = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text5,NULL,screen,&maxRect);
        sprintf(c,"Lives: %d",lives);
        text6 = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(text6,NULL,screen,&liveRect);
        SDL_FreeSurface(text);
        SDL_FreeSurface(text2);
        SDL_FreeSurface(text3);
        SDL_FreeSurface(text4);
        SDL_FreeSurface(text5);
        SDL_FreeSurface(text6);

   }
        if(pause == 1)
        {
           SDL_BlitSurface(background,NULL,screen,NULL);
           player1.Show(NULL,NULL);
           enemy1.Show(NULL,NULL);
           enemy11.Show(NULL,NULL);
           enemy2.Show(NULL,NULL);
           enemy3.Show(NULL,NULL);
           move[0] = 0;
           move[1] = 0;
           move[2] = 0;
           move[3] = 0;
           SDL_Surface *text,*text2;
           text = TTF_RenderText_Solid(fontMenu,"Resume",color5);
           SDL_BlitSurface(text,NULL,screen,&resume);
           text2 = TTF_RenderText_Solid(fontMenu,"Exit",color6);
           SDL_BlitSurface(text2,NULL,screen,&exit);
           SDL_FreeSurface(text);
           SDL_FreeSurface(text2);
        }
        if(pause == 3)
        {
            char c[5];
            sprintf(c,"HP: %d",player1.hp);
            SDL_Surface *text,*text2,*text3,*text4,*text5,*text6;
            text = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(background,NULL,screen,NULL);
            SDL_BlitSurface(text,NULL,screen,&hpRect);
            sprintf(c,"Score: %d",score);
            text2 = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(text2,NULL,screen,&scoreRect);
            sprintf(c,"Bullets: %d",BulletAmount);
            text3 = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(text3,NULL,screen,&bulletRect);
            sprintf(c,"Mines: %d",BombAmount);
            text4 = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(text4,NULL,screen,&bombRect);
            sprintf(c,"Max Score: %d",maxScore);
            text5 = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(text5,NULL,screen,&maxRect);
            sprintf(c,"Lives: %d",lives);
            text6 = TTF_RenderText_Solid(font,c,color);
            SDL_BlitSurface(text6,NULL,screen,&liveRect);
            player1.Show(NULL,NULL);
            enemy1.Show(NULL,NULL);
            enemy11.Show(NULL,NULL);
            enemy2.Show(NULL,NULL);
            enemy3.Show(NULL,NULL);
            SDL_FreeSurface(text);
            SDL_FreeSurface(text2);
            SDL_FreeSurface(text3);
            SDL_FreeSurface(text4);
            SDL_FreeSurface(text5);
            SDL_FreeSurface(text6);

        }

    prompt.update(screen);
    if(setBack == true)
    {
        fstream file2;
        file2.open("Saves/set.txt");
        file2 >> score;
        file2 >> type;
        file2 >> player1.hp;
        file2 >> lives;
        file2 >> player1.getRect()->x;
        file2 >> player1.getRect()->y;
        file2 >> enemy1.getRect()->x;
        file2 >> enemy1.getRect()->y;
        file2 >> enemy2.getRect()->x;
        file2 >> enemy2.getRect()->y;
        file2 >> enemy3.getRect()->x;
        file2 >> enemy3.getRect()->y;
        file2 >> enemy11.getRect()->x;
        file2 >> enemy11.getRect()->y;
        file2 >> BulletAmount;
        file2 >> BombAmount;
        file2.close();
        setBack = false;
    }

    if(Gamestate == 4)
    {
        SDL_Surface *helpScreen = SDL_LoadBMP("Images/help.bmp");
        SDL_BlitSurface(helpScreen,NULL,screen,NULL);
        SDL_FreeSurface(helpScreen);
    }

    if(Gamestate == 5)
    {
        SDL_BlitSurface(menuBack,NULL,screen,NULL);
        textEasy1 = TTF_RenderText_Solid(fontMenu,"Recruit",color12);
        textMedium1 = TTF_RenderText_Solid(fontMenu,"Carnage",color11);
        textHard1 = TTF_RenderText_Solid(fontMenu,"Insane!",color10);
        SDL_BlitSurface(textEasy1,NULL,screen,&hard1);
        SDL_BlitSurface(textMedium1,NULL,screen,&medium1);
        SDL_BlitSurface(textHard1,NULL,screen,&easy1);
        SDL_FreeSurface(textEasy1);
        SDL_FreeSurface(textMedium1);
        SDL_FreeSurface(textHard1);
    }

    if(Gamestate == 3)
    {
        int accuracy = bulletHit * 100 / bulletsShooted;
        SDL_BlitSurface(menuBack,NULL,screen,NULL);
        SDL_Surface *MaxScoreSur;
        SDL_Surface *AccSur;
        SDL_Surface *BullSur;
        SDL_Surface *BmbSur;
        char c[5];
        sprintf(c,"MaxScore: %d",maxScore);
        MaxScoreSur = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(MaxScoreSur,NULL,screen,&MaxScore);
        sprintf(c,"Bullets Shooted: %d",bulletsShooted);
        BullSur = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(BullSur,NULL,screen,&Bull);
        sprintf(c,"Bombs Shooted: %d",bombsShooted);
        BmbSur = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(BmbSur,NULL,screen,&Bmb);
        sprintf(c,"Accuracy: %d",accuracy);
        AccSur = TTF_RenderText_Solid(font,c,color);
        SDL_BlitSurface(AccSur,NULL,screen,&Acc);
        SDL_FreeSurface(AccSur);
        SDL_FreeSurface(BmbSur);
        SDL_FreeSurface(BullSur);
        SDL_FreeSurface(MaxScoreSur);
    }

   //if im in the menu
   if(Gamestate == 1)
   {
        SDL_BlitSurface(menuBack,NULL,screen,NULL);
        startGameSur = TTF_RenderText_Solid(fontMenu,"Start Carnage",color1);
        statsSur = TTF_RenderText_Solid(fontMenu,"Stats",color2);
        helpSur = TTF_RenderText_Solid(fontMenu,"Help",color3);
        difficultSur = TTF_RenderText_Solid(fontMenu,"Difficultie",color4);
        SDL_BlitSurface(startGameSur,NULL,screen,&startGame);
        SDL_BlitSurface(statsSur,NULL,screen,&stats);
        SDL_BlitSurface(helpSur,NULL,screen,&help);
        SDL_BlitSurface(difficultSur,NULL,screen,&difficult);
        SDL_FreeSurface(startGameSur);
        SDL_FreeSurface(statsSur);
        SDL_FreeSurface(helpSur);
        SDL_FreeSurface(difficultSur);
   }

    char *c;
    int fps = ( FPS/(SDL_GetTicks() - start)*10);
    sprintf(c,"FPS: %d",fps);
    fpsSur = TTF_RenderText_Solid(fontMin,c,color);
    SDL_BlitSurface(fpsSur,NULL,screen,&fpsRect);
    SDL_FreeSurface(fpsSur);

    SDL_BlitSurface(cursor,NULL,screen,&cursorRect);

    if (1000/FPS>SDL_GetTicks()-start)
    {
        SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
    }
    SDL_Flip(screen);

    }
    SDL_FreeSurface(bulletimage);
    SDL_FreeSurface(bombimage);
    TTF_CloseFont(font);
    SDL_FreeSurface(background);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
