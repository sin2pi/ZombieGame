#include "LuaPrompt.h"
#include <lua.hpp>
#include <SDL.h>
#include <SDL_ttf.h>

extern lua_State *L;
extern SDL_Event event;
extern TTF_Font* font2;
extern SDL_Color Green;

SDL_Surface* Prompt;
LuaPrompt::LuaPrompt()
{
    str = "";

}
//Lua Prompt Destructor
LuaPrompt::~LuaPrompt()
{

}
//Lua Prompt Update Text/Render Function
void LuaPrompt::update(SDL_Surface* screen)
{

    if(PromptActive == true)
    {
        if(Prompt != NULL)
        {
            SDL_Rect rect;
            rect.x = 10;
            rect.y = 300;
            SDL_BlitSurface(Prompt,NULL,screen,&rect);
            SDL_Surface(Prompt);

        }

    }

}
//Lua Prompt Input/Text Updating Function
void LuaPrompt::handle_input()
{
    SDL_EnableUNICODE( SDL_ENABLE );
    if (event.type == SDL_KEYDOWN)
    {

        if( event.key.keysym.unicode == (Uint16)' ' )
        {
            str += (char)event.key.keysym.unicode;
        }
        else if( ( event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9' ) )
        {
            str += (char)event.key.keysym.unicode;
        }
        else if( ( event.key.keysym.unicode >= (Uint16)'A' ) && ( event.key.keysym.unicode <= (Uint16)'Z' ) )
        {
            str += (char)event.key.keysym.unicode;
        }
        else if( ( event.key.keysym.unicode >= (Uint16)'a' ) && ( event.key.keysym.unicode <= (Uint16)'z' ) )
        {
            str += (char)event.key.keysym.unicode;
        }
        else if( event.key.keysym.unicode == 0x0028 || event.key.keysym.unicode == 0x0029 || event.key.keysym.unicode == 0x002C || event.key.keysym.unicode == 0x0022 || event.key.keysym.unicode == 0x002F || event.key.keysym.unicode == 0x002B || event.key.keysym.unicode == 0x002D || event.key.keysym.unicode == 0x002E || event.key.keysym.unicode == 0x003D || event.key.keysym.unicode == 0x003A)
        {
            str += (char)event.key.keysym.unicode;
        }

        if( ( event.key.keysym.sym == SDLK_BACKSPACE ) && ( str.length() != 0 ) )
        {
            str.erase(str.length()-1);
        }

        if ( event.key.keysym.sym == SDLK_RETURN )
        {
            int ret = luaL_dostring(L,str.c_str());
            if(ret != 0)
            {
                const char *out = lua_tostring(L,-1);
                lua_pop(L,1);
            }
            str = "";
        }

        Prompt = TTF_RenderText_Shaded(font2,str.c_str(),Green, {0,0,0});
        if(Prompt != NULL)
        {
            SDL_Surface(Prompt);
        }

    }
}
