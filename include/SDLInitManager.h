#pragma once
#include <SDL.h>

class SDLInitManager
{
public:
    SDLInitManager();
    ~SDLInitManager(void);

    SDL_Renderer* GetRenderer();
    SDL_Window* GetWindow();
    SDL_Event* GetMainSDLEvent();

    void Begin();
    void End();

private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
    SDL_Event* m_sdl_event;
};

