#include "Game.h"
#include <string>
#include <cstdlib>

void allocate_play_texture();

static constexpr std::string getDefaultFontPath() {
    // FIXME: Implement a way to get a default monospaced font.
    // return "/usr/share/fonts/truetype/freefont/FreeMono.ttf";
    return "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf";
}

Game::Game() {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Panic: SDL initialization failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        printf("Panic: Failed to initialize SDL_ttf, abort.\n");
        exit(EXIT_FAILURE);
    }

    _window = SDL_CreateWindow("Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!_window) {
        printf("panic: Window creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    if (!_renderer) {
        printf("panic: SDL Renderer creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    _font = TTF_OpenFont(getDefaultFontPath().c_str(), 24);
    if (!_font) {
        printf("Panic: Failed to load font, abort.\n");
        exit(EXIT_FAILURE);
    }

    _event = new SDL_Event();
}

std::pair<int, int> Game::GetTextureDimensions(SDL_Texture* texture) {
    int width = 0;
    int height = 0;
    if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) {
        printf("Panic: Failed to query texture, abort.\n");
        exit(EXIT_FAILURE);
    }

    return std::pair(width, height);
}

void Game::AllocateTextTexture(SDL_Color color, std::string text) {
    SDL_Surface* surface = TTF_RenderText_Solid(
        _font,
        text.c_str(),
        color
    );
    if (!surface) {
        printf("Panic: Failed to obtain surface, abort.\n");
        exit(EXIT_FAILURE);
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!text_texture) {
        printf("Panic: Failed to create texture for text, abort.\n");
        exit(EXIT_FAILURE);
    }
    _textures.push_back(text_texture);
}

Game::~Game() {
    _event = NULL;
}

SDL_Event* Game::GetEvent() {
    SDL_PollEvent(_event);
    return _event;
}

void Game::RenderScene() {
    // FIXME: Texture rectangles should be abstracted.
    // FIXME: All textures and their rectangles should be drawn.

    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);

    SDL_Rect texture_rect;
    texture_rect.x = 0;
    texture_rect.y = 0;
    texture_rect.w = SCREEN_WIDTH;
    texture_rect.h = SCREEN_WIDTH;
    SDL_RenderCopy(_renderer, _textures[0], NULL, &texture_rect);

    std::pair<int, int> play_button_dims = GetTextureDimensions(_textures[1]);
    SDL_Rect play_button_rect;
    int play_width = std::get<0>(play_button_dims);
    int play_height = std::get<1>(play_button_dims);
    play_button_rect.x = SCREEN_WIDTH/2 - (play_width/2);
    play_button_rect.y = SCREEN_HEIGHT/2 - 1;
    play_button_rect.w = play_width;
    play_button_rect.h = play_height;

    SDL_RenderCopy(_renderer, _textures[1], NULL, &play_button_rect);

    SDL_RenderPresent(_renderer);
}

void Game::LoadTexture(std::string path) {
    SDL_Texture* texture = IMG_LoadTexture(_renderer, path.c_str());
    if (texture == NULL){
        printf("Panic: Failed to load texture at %s.\n", path.c_str());
        exit(EXIT_FAILURE);
	} else {
        _textures.push_back(texture);
    }
}
