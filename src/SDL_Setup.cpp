#include "SDL_Setup.h"

CSDL_Setup::CSDL_Setup(bool* quit, int ScreenWidth, int ScreenHeight){
	SDL_Init(SDL_INIT_VIDEO);

	window = NULL;
	window = SDL_CreateWindow("Forged Memories v2.3", 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if (window == NULL){
		cout << "Window couldn't be created" << endl;
		*quit = true;
	}

	renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 

	mainEvent = new SDL_Event();
}

CSDL_Setup::~CSDL_Setup(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	delete mainEvent;
}

SDL_Renderer* CSDL_Setup::GetRenderer(){
	return renderer;
}

SDL_Event* CSDL_Setup::GetMainEvent(){
	return mainEvent;
}

void CSDL_Setup::Begin(){
	SDL_PollEvent(mainEvent);
	SDL_RenderClear(renderer);
}

void CSDL_Setup::End(){
	SDL_RenderPresent(renderer);

}

SDL_Window* CSDL_Setup::GetWindow(){
	return window;
}