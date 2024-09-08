#include "Game.h"
#include <string>

Game::Game() {
    SDL_Init(SDL_INIT_VIDEO);
    _window = SDL_CreateWindow("Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (_window == NULL) {
        printf("panic: Window creation failed, abort.\n");
    }
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    _event = new SDL_Event();
}

Game::~Game() {
    _event = NULL;
}

SDL_Event* Game::GetEvent() {
    SDL_PollEvent(_event);
    return _event;
}

void Game::Draw() {
    SDL_Rect texture_rect;
    texture_rect.x = 0;
    texture_rect.y = 0;
    texture_rect.w = SCREEN_WIDTH;
    texture_rect.h = SCREEN_WIDTH;

    // FIXME: Texture rectangles should be abstracted.
    // FIXME: All textures and their rectangles should be drawn.

    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _textures[0], NULL, &texture_rect); 
    SDL_RenderPresent(_renderer); //updates the renderer    
}

void Game::LoadTexture(std::string path) {
    SDL_Texture* texture = IMG_LoadTexture(_renderer, path.c_str());
    if (texture == NULL){
        printf("Panic: Failed to load texture at %s.\n", path.c_str());
	} else {
        _textures.push_back(texture);
    }
}
