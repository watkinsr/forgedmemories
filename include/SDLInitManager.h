#pragma once
#include <SDL.h>

class SDLInitManager
{
public:
    SDLInitManager();
    ~SDLInitManager(void);

    SDL_Renderer* GetRenderer();
    SDL_Event* GetMainEvent();
    SDL_Window* GetWindow();

    void Begin();
    void End();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};

