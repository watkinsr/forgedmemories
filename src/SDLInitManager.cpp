#include "../include/SDLInitManager.h"
#include "iostream"

using namespace std;

SDLInitManager::SDLInitManager() {
	SDL_Init(SDL_INIT_VIDEO);

	m_window = NULL;
	m_window = SDL_CreateWindow("Game", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (m_window == NULL) {
		cout << "panic: Window failed to create at SDL_CreateWindow" << endl;
	}

	m_renderer = NULL;
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
    m_sdl_event = new SDL_Event();
}

SDLInitManager::~SDLInitManager(){
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
    delete m_sdl_event;
}

SDL_Renderer* SDLInitManager::GetRenderer(){
	return m_renderer;
}

SDL_Event* SDLInitManager::GetMainSDLEvent(){
    return m_sdl_event;
}

void SDLInitManager::Begin(){
    SDL_PollEvent(m_sdl_event);
    SDL_RenderClear(m_renderer);
}

void SDLInitManager::End(){
	SDL_RenderPresent(m_renderer);
}

SDL_Window* SDLInitManager::GetWindow(){
	return m_window;
}