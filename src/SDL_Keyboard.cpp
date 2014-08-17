#include "stdafx.h"
#include "SDL_Keyboard.h"



SDL_Keyboard::SDL_Keyboard(void)
{
}


SDL_Keyboard::~SDL_Keyboard(void)
{
}

const Uint8* keyboard;

void update()
{
    SDL_PumpEvents();
    keyboard = SDL_GetKeyboardState(NULL);
}
