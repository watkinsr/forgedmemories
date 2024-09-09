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

void Game::IncrementSceneStack() {
    scene_t scene;
    if (_scene_stack_idx < SCENE_STACK_MAX_SIZE) {
	_scene_stack_idx++;
	_scenes.push_back(scene);
	_scenes[_scene_stack_idx].textures = std::vector<SDL_Texture*>();
	_scenes[_scene_stack_idx].texture_rects = std::vector<SDL_Rect>();
    }
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

void Game::AllocateTextTexture(const uint8_t scene_idx, const SDL_Color color, const std::string text, const uint32_t x, const uint32_t y) {
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

    _scenes[scene_idx].textures.push_back(text_texture);
    SDL_Rect texture_rect;
    std::pair<int, int> texture_dims = GetTextureDimensions(text_texture);
    const int width = std::get<0>(texture_dims);
    const int height = std::get<1>(texture_dims);
    texture_rect.x = x - width/2;
    texture_rect.y = y;
    texture_rect.w = width;
    texture_rect.h = height;
    _scenes[scene_idx].texture_rects.push_back(texture_rect);
}

Game::~Game() {
    _event = NULL;
}

SDL_Event* Game::GetEvent() {
    SDL_PollEvent(_event);
    return _event;
}

void Game::InitDefaultScene() {
    scene_t scene;
    _scenes.push_back(scene);
    if (_scenes.size() < 1) {
	printf("Panic: Failed to init scene.\n");
	exit(EXIT_FAILURE);
    }
    _scenes[0].textures = std::vector<SDL_Texture*>();
    _scenes[0].texture_rects = std::vector<SDL_Rect>();
    LoadTexture(0, "assets/menu.gif", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    AllocateTextTexture(0, {255,255,255,255}, "Play", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 1);
}

void Game::RenderScene() {
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);

    for (uint8_t i = 0; i < _scenes[_scene_stack_idx].textures.size(); ++i) {
	SDL_Texture* texture = _scenes[_scene_stack_idx].textures[i];
	SDL_Rect rect = _scenes[_scene_stack_idx].texture_rects[i];
	SDL_RenderCopy(_renderer, texture, NULL, &rect);
    }

    SDL_RenderPresent(_renderer);
}

void Game::LoadTexture(const uint8_t scene_idx, const std::string path, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h) {
    SDL_Texture* texture = IMG_LoadTexture(_renderer, path.c_str());
    if (texture == NULL) {
        printf("Panic: Failed to load texture at %s.\n", path.c_str());
        exit(EXIT_FAILURE);
    } else {
	_scenes[scene_idx].textures.push_back(texture);
	SDL_Rect texture_rect;
	texture_rect.x = x;
	texture_rect.y = y;
	texture_rect.w = w;
	texture_rect.h = h;
	_scenes[scene_idx].texture_rects.push_back(texture_rect);
    }
}
