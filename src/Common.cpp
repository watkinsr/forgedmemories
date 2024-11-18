#include "Common.h"

void Common::SetupSDL() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Panic: SDL initialization failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        printf("Panic: Failed to initialize SDL_ttf, abort.\n");
        exit(EXIT_FAILURE);
    }

    _window = SDL_CreateWindow(_app_name.c_str(), -1, -1, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!_window) {
        printf("panic: Window creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    _renderer = SDL_CreateRenderer(_window, -1,
                         SDL_RENDERER_ACCELERATED);
    if (!_renderer) {
        printf("panic: SDL Renderer creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    _screen_surface = SDL_GetWindowSurface(_window);
    SDL_UpdateWindowSurface(_window);

    for (uint8_t i = 0; i < DEFAULT_FONT_ARRAY_LEN; ++i) {
        _font = TTF_OpenFont(DEFAULT_FONTS[i].data(), 24);
        if (_font) break;
        if (!_font && i == DEFAULT_FONT_ARRAY_LEN - 1) {
            printf("Panic: Failed to load a default font, abort.\n");
            exit(EXIT_FAILURE);
        }
    }

    // SDL_SetHint("SDL_HINT_TOUCH_MOUSE_EVENTS", "1");
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

    _event = new SDL_Event();
}

void Common::AddScene(std::vector<gametexture_t> scene) {
    _scene_texture_locations.push_back(scene);
}

Common::~Common() {
    _event = NULL;
}

Common::Common(std::string app_name) {
    _app_name = app_name;
    SetupSDL();
}

SDL_Event* Common::GetEvent() {
    SDL_PollEvent(_event);
    return _event;
}

void Common::AllocateScene(bool incrementStackIdx) {
    if (_scene_stack_idx + 1 < SCENE_STACK_MAX_SIZE) {
        if (incrementStackIdx) _scene_stack_idx++;
        scene_t scene;
        _scenes.push_back(scene);
        LOG_INFO("Common::AllocateScene() => Stack size: %li.", _scenes.size());
        LOG_INFO("Common::AllocateScene() => Stack index: %i.",
                  _scene_stack_idx);
        LOG_INFO("Common::AllocateScene() => Allocate %li textures for Scene: %i",
        _scene_texture_locations[_scene_stack_idx].size(), _scene_stack_idx);

        _scenes[_scene_stack_idx].textures = std::vector<SDL_Texture*>();
        _scenes[_scene_stack_idx].texture_src_rects = std::vector<SDL_Rect>();
        _scenes[_scene_stack_idx].texture_dst_rects = std::vector<SDL_Rect>();
        _scenes[_scene_stack_idx].tags = std::vector<uint8_t>();
        for (uint8_t i = 0; i < _scene_texture_locations[_scene_stack_idx].size(); ++i) {
            LoadTexture(_scene_stack_idx, _scene_texture_locations[_scene_stack_idx][i]);
        }
        // FIXME: We might need to destroy the old textures.
    }
}

void Common::LoadTexture(const uint8_t scene_idx, gametexture_t game_texture) {
    if (isRectTexture(game_texture.tag)) {
        _scenes[scene_idx].texture_src_rects.push_back(game_texture.src_rect);
        _scenes[scene_idx].texture_dst_rects.push_back(game_texture.dst_rect);
        _scenes[scene_idx].tags.push_back(game_texture.tag);
        _scenes[scene_idx].colors.push_back(game_texture.color);
        LOG_INFO("Allocated rect");
    } else if (isTextTexture(game_texture.tag)) {
        SDL_Surface* surface = TTF_RenderText_Solid(
            _font,
            game_texture.text_or_uri.c_str(),
            game_texture.color
        );
        if (!surface) {
            printf("Panic: Failed to obtain surface, abort.\n");
            SDL_Quit();
            exit(EXIT_FAILURE);
        }

        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        if (!text_texture) {
            printf("Panic: Failed to create texture for text, abort.\n");
            exit(EXIT_FAILURE);
        }
        pair<int, int> texture_dims = GetTextureDimensions(text_texture);
        const int width = std::get<0>(texture_dims);
        const int height = std::get<1>(texture_dims);
        game_texture.dst_rect.w = width;
        game_texture.dst_rect.h = height;

        _scenes[scene_idx].textures.push_back(text_texture);
        _scenes[scene_idx].texture_src_rects.push_back(game_texture.src_rect);
        _scenes[scene_idx].texture_dst_rects.push_back(game_texture.dst_rect);
        _scenes[scene_idx].tags.push_back(game_texture.tag);
        // LOG_INFO("Loaded text texture: %s", game_texture.text_or_uri.c_str());
    } else if (isImageTexture(game_texture.tag)) {
        LOG_INFO("Game::LoadTexture(...) => Received image texture");
        const char* path = game_texture.text_or_uri.c_str();
        SDL_Texture* texture = IMG_LoadTexture(_renderer, path);
        if (texture == NULL) {
            printf("Panic: Failed to load image texture at %s.\n", path);
            SDL_Quit();
            exit(EXIT_FAILURE);
        } else {
            _scenes[scene_idx].textures.push_back(texture);
            _scenes[scene_idx].texture_src_rects.push_back(game_texture.src_rect);
            _scenes[scene_idx].texture_dst_rects.push_back(game_texture.dst_rect);
            _scenes[scene_idx].tags.push_back(game_texture.tag);
        }
        LOG_INFO("Loaded image texture at %s", path);
    } else if (isSpriteTexture(game_texture.tag)) {
        const char* path = game_texture.text_or_uri.c_str();
        SDL_Texture* texture = IMG_LoadTexture(_renderer, path);
        if (texture == NULL) {
            printf("Panic: Failed to load sprite texture at %s.\n", path);
            SDL_Quit();
            exit(EXIT_FAILURE);
        } else {
            _scenes[scene_idx].textures.push_back(texture);
            _scenes[scene_idx].texture_src_rects.push_back(game_texture.src_rect);
            _scenes[scene_idx].texture_dst_rects.push_back(game_texture.dst_rect);
            _scenes[scene_idx].tags.push_back(game_texture.tag);
        }
        LOG_INFO("Loaded sprite texture at %s", path);
    } else {
        LOG_INFO("Game::LoadTexture(...) => Incorrect tag applied to Game Texture. Abort !");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

bool Common::isEnemySpriteTexture(uint8_t tag) {
    uint8_t combined_tag = (SPRITE_TAG | ENEMY_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}

bool Common::isPlayerSpriteTexture(uint8_t tag) {
    if (isEnemySpriteTexture(tag)) return false;
    uint8_t combined_tag = (SPRITE_TAG | PLAYER_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}

bool Common::isBackgroundSpriteTexture(uint8_t tag) {
    if (isEnemySpriteTexture(tag)) return false;
    uint8_t combined_tag = (SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}
bool Common::isTextTexture(uint8_t tag) {
    if (isPlayerSpriteTexture(tag) || isEnemySpriteTexture(tag)) return false;
    return (tag & TEXT_TAG) == TEXT_TAG;
}
bool Common::isImageTexture(uint8_t tag) {
    if (isBackgroundSpriteTexture(tag) ||
    isEnemySpriteTexture(tag)) return false;
    return (tag & IMAGE_TAG) == IMAGE_TAG;
}
bool Common::isSpriteTexture(uint8_t tag) {
    // LOG_INFO("isSpriteTexture(%u)", tag);
    return (tag & SPRITE_TAG) == SPRITE_TAG ||
    isPlayerSpriteTexture(tag) ||
    isBackgroundSpriteTexture(tag) ||
    isEnemySpriteTexture(tag);
}

bool Common::isRectTexture(uint8_t tag) {
    return (tag & RECT_TAG) == RECT_TAG;
}

std::pair<int, int> Common::GetTextureDimensions(SDL_Texture* texture) {
    int width = 0;
    int height = 0;
    if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) {
        printf("Panic: Failed to query texture, abort.\n");
        exit(EXIT_FAILURE);
    }

    return std::pair(width, height);
}

SDL_Renderer* Common::GetRenderer() { return _renderer; }
SDL_Window* Common::GetWindow() { return _window; }
uint8_t Common::GetSceneStackIdx() { return _scene_stack_idx; }
scene_t* Common::GetCurrentScene() { return &_scenes[_scene_stack_idx]; }
