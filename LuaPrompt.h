#ifndef LUAPROMPT_H
#define LUAPROMPT_H
#include <SDL.h>
#include <string>
class LuaPrompt
{
public:
    //Constructor & Deconstructor
    LuaPrompt();
    ~LuaPrompt();

    bool PromptActive;
    //String Holder
    std::string str;
    //Update Prompt Text
    void update(SDL_Surface*);
    //Handle Input To Prompt
    void handle_input();
};

#endif // LUAPROMPT_H
