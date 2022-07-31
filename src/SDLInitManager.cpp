#include "SDLInitManager.h"
#include "iostream"

using namespace std;

SDLInitManager::SDLInitManager() {
	SDL_Init(SDL_INIT_VIDEO);

	window = NULL;
	window = SDL_CreateWindow("Game", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		cout << "panic: Window failed to create at SDL_CreateWindow" << endl;
	}

	renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDLInitManager::~SDLInitManager(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	delete mainEvent;
}

SDL_Renderer* SDLInitManager::GetRenderer(){
	return renderer;
}

void SDLInitManager::Begin(){
}

void SDLInitManager::End(){
	SDL_RenderPresent(renderer);

}

SDL_Window* SDLInitManager::GetWindow(){
	return window;
}