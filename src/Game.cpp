#include "Game.h"
#include <string>
#include <cstdlib>

constexpr uint8_t DEFAULT_FONT_ARRAY_LEN = 2;
constexpr uint8_t PLAYER_WIDTH = 48;
constexpr uint8_t PLAYER_HEIGHT = 48;

constexpr uint8_t MAP[4][4] = {
    {1, 0, 0, 1},
    {0, 0, 0, 0},
    {1, 0, 1, 0},
    {0, 0, 0, 0}
};

constexpr std::array<std::string_view, DEFAULT_FONT_ARRAY_LEN> DEFAULT_FONTS = {
    "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
    "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf"
};

#define TEXT_TAG 1 << 0
#define IMAGE_TAG 1 << 1
#define SPRITE_TAG 1 << 2
#define PLAYER_SPRITE_FLAG 1 << 0
#define BACKGROUND_SPRITE_FLAG 1 << 1

constexpr bool isPlayerSpriteTexture(uint8_t tag) {
    return (tag & (SPRITE_TAG | PLAYER_SPRITE_FLAG)) == (SPRITE_TAG | PLAYER_SPRITE_FLAG);
}
constexpr bool isBackgroundSpriteTexture(uint8_t tag) {
    return (tag & (SPRITE_TAG | BACKGROUND_SPRITE_FLAG)) == (SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
}
constexpr bool isTextTexture(uint8_t tag) {
    if (isPlayerSpriteTexture(tag)) return false;
    return (tag & TEXT_TAG) == TEXT_TAG;
}
constexpr bool isImageTexture(uint8_t tag) {
    if (isBackgroundSpriteTexture(tag)) return false;
    return (tag & IMAGE_TAG) == IMAGE_TAG;
}
constexpr bool isSpriteTexture(uint8_t tag) {
    return (tag & SPRITE_TAG) == SPRITE_TAG || isPlayerSpriteTexture(tag) || isBackgroundSpriteTexture(tag);
}

const bool Game::IsColliding(uint16_t x, uint16_t y) {

    uint16_t collide_y = (y + ((PLAYER_HEIGHT/4) * 3.5));
    uint16_t collide_x_right = (x + ((PLAYER_WIDTH/4) * 3.5));

    if (collide_x_right >= SCREEN_WIDTH || x <= 0) return false;

    uint8_t segment_x_left = (uint8_t)((x + PLAYER_WIDTH/4)/(SCREEN_WIDTH/4));
    uint8_t segment_x_right = (uint8_t)(collide_x_right/(SCREEN_WIDTH/4));
    uint8_t segment_y = (uint8_t)(collide_y/(SCREEN_HEIGHT/4));
    const bool is_colliding = MAP[segment_y][segment_x_left] == 1 ||
    MAP[segment_y][segment_x_right] == 1;
    if (is_colliding) LOG_INFO("Collision !");
    return is_colliding;
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

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        printf("panic: SDL Renderer creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    for (uint8_t i = 0; i < DEFAULT_FONT_ARRAY_LEN; ++i) {
        _font = TTF_OpenFont(DEFAULT_FONTS[i].data(), 24);
        if (_font) break;
        if (!_font && i == DEFAULT_FONT_ARRAY_LEN - 1) {
            printf("Panic: Failed to load a default font, abort.\n");
            exit(EXIT_FAILURE);
        }
    }

    _event = new SDL_Event();

    _SetTextureLocations();
    AllocateScene(false);
}

void Game::_SetTextureLocations() {
    const vector<gametexture_t> SCENE_1 = {
        { .text_or_uri = "Forged Memories",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {SCREEN_WIDTH/2.5 - 20, 50, 0, 0},
          .color = {255,255,255,255},
          .tag = TEXT_TAG
        },
        { .text_or_uri = "<SPC> to play",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {SCREEN_WIDTH/2.5, SCREEN_HEIGHT/2 - 1, 0, 0},
          .color = {255,255,255,255},
          .tag = TEXT_TAG
        }
    };
    const vector<gametexture_t> SCENE_2 = {
        { .text_or_uri = "assets/bg/Berry Garden.png",
          .src_rect = {0, 0, 16, 16},
          .dst_rect = {0, 0, 64, 128},
          .color = {0,0,0,0},
          .tag = SPRITE_TAG | BACKGROUND_SPRITE_FLAG
        },
        { .text_or_uri = "assets/player.png",
          .src_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
          .dst_rect = {_player_x, _player_y, PLAYER_WIDTH, PLAYER_HEIGHT},
          .color = {0,0,0,0},
          .tag = SPRITE_TAG | PLAYER_SPRITE_FLAG
        }
    };
    _scene_texture_locations.push_back(SCENE_1);
    _scene_texture_locations.push_back(SCENE_2);
    LOG_INFO("Game::SetTextureLocations() => Allocated %li Scene Texture Locations.", _scene_texture_locations.size());
}

const bool Game::AfterMainMenu() {
    return _scene_stack_idx + 1 >= SCENE_STACK_MAX_SIZE;
}

void Game::AllocateScene(bool incrementStackIdx) {
    if (_scene_stack_idx + 1 < SCENE_STACK_MAX_SIZE) {
        if (incrementStackIdx) _scene_stack_idx++;
        scene_t scene;
        _scenes.push_back(scene);
        LOG_INFO("Game::AllocateScene() => Scene stack size: %li.", _scenes.size());
        LOG_INFO("Game::AllocateScene() => Scene stack index: %li.", _scene_stack_idx);
        LOG_INFO("Game::AllocateScene() => There are %li textures to allocate for Scene: %i", _scene_texture_locations[_scene_stack_idx].size(), _scene_stack_idx);
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

std::pair<int, int> Game::GetTextureDimensions(SDL_Texture* texture) {
    int width = 0;
    int height = 0;
    if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) {
        printf("Panic: Failed to query texture, abort.\n");
        exit(EXIT_FAILURE);
    }

    return std::pair(width, height);
}

Game::~Game() {
    _event = NULL;
}

SDL_Event* Game::GetEvent() {
    SDL_PollEvent(_event);
    return _event;
}

void Game::RenderScene() {
    float tick = SDL_GetTicks();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    else SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);

    if (_player_y <= 2) {
        _player_y = SCREEN_HEIGHT - 10;
    } else if (SCREEN_HEIGHT - _player_y <= 2) {
        _player_y = 10;
    } else if (_player_x <= 2) {
        _player_x = SCREEN_WIDTH - 10;
    } else if (SCREEN_WIDTH - _player_x <= 2) {
        _player_x = 10;
    }

    for (uint8_t i = 0; i < _scenes[_scene_stack_idx].textures.size(); ++i) {
        SDL_Texture* texture = _scenes[_scene_stack_idx].textures[i];
        SDL_Rect src_rect = _scenes[_scene_stack_idx].texture_src_rects[i];
        SDL_Rect dst_rect = _scenes[_scene_stack_idx].texture_dst_rects[i];
        uint8_t tag = _scenes[_scene_stack_idx].tags[i];
        if (src_rect.w == 0) {
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        } else {
            if (isBackgroundSpriteTexture(tag)) {
                dst_rect.w = 16;
                dst_rect.h = 16;
                for (uint16_t i = 0; i < SCREEN_WIDTH; i+=16) {
                    dst_rect.x = i;
                    for (uint16_t j = 0; j < SCREEN_HEIGHT; j+=16) {
                        dst_rect.y = j;
                        uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (MAP[map_segment_y][map_segment_x] == 1) {
                            src_rect.x = 49;
                            src_rect.y = 48;
                        } else {
                            src_rect.x = 17;
                            src_rect.y = 64;
                        }
                        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                    }
                }
            } else if (isPlayerSpriteTexture(tag)) {
                if (_player_state == player_state_t::MOVING) {
                    _deltaTick = SDL_GetTicks() - _tick;
                    if (_deltaTick >= 100) {
                        _deltaTick = _deltaTick % 100;
                        _tick = SDL_GetTicks();
                        src_rect.x = src_rect.x == 0 ? PLAYER_WIDTH * 2 : 0;
                    }
                    src_rect.y = 0;
                    _scenes[_scene_stack_idx].texture_src_rects[i] = src_rect;                    
                } else if (_player_state == player_state_t::STOPPED) {
                    src_rect.x = PLAYER_WIDTH;
                    src_rect.y = 0;
                } else if (_player_state == player_state_t::ATTACK) {
                    src_rect.x = PLAYER_WIDTH;
                    src_rect.y = PLAYER_HEIGHT;
                }

                dst_rect.x = GetPlayerX();
                dst_rect.y = GetPlayerY();
            }
            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
        }
    }

    SDL_RenderPresent(_renderer);

    if (_scenes[_scene_stack_idx].textures.size() == 3) {
        // Remove the FPS texture.
        _scenes[_scene_stack_idx].textures.pop_back();
    }

    uint32_t deltaTick = SDL_GetTicks() - tick;
    uint32_t fps = 1000.0 / deltaTick;
    gametexture_t fps_texture = {
        .text_or_uri = "FPS: " + std::to_string(fps),
        .src_rect = {0, 0, 0, 0},
        .dst_rect = {20, 20, 0, 0},
        .color = {255,255,255,255},
        .tag = TEXT_TAG
    };

    LoadTexture(_scene_stack_idx, fps_texture);


}

void Game::LoadTexture(const uint8_t scene_idx, gametexture_t game_texture) {
    if (isTextTexture(game_texture.tag)) {
        SDL_Surface* surface = TTF_RenderText_Solid(
            _font,
            game_texture.text_or_uri.c_str(),
            game_texture.color
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
            printf("Panic: Failed to load texture at %s.\n", path);
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
            printf("Panic: Failed to load texture at %s.\n", path);
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
