#include "Game.h"
#include <string>
#include <cstdlib>

constexpr uint8_t DEFAULT_FONT_ARRAY_LEN = 2;

// Center
#define MAP1 {\
    {1, 0, 0, 1},\
    {0, 0, 2, 0},\
    {0, 0, 0, 0},\
    {1, 1, 1, 1}\
}

// Left of center
#define MAP2 {\
    {1, 1, 1, 1},\
    {1, 0, 2, 0},\
    {1, 0, 1, 0},\
    {1, 1, 1, 1}\
}

// Right of center
#define MAP3 {\
    {1, 1, 1, 1},\
    {0, 0, 2, 1},\
    {0, 0, 1, 1},\
    {1, 1, 1, 1}\
}

// Above center
#define MAP4 {\
    {1, 1, 1, 1},\
    {1, 0, 2, 1},\
    {1, 0, 1, 1},\
    {1, 0, 0, 1}\
}

// One above, one left, one right, one below.
const uint8_t MAP_SIZE = 4;

// Current player map.
uint8_t MAP[4][4] = MAP1;

uint8_t map_idx = 0;

constexpr uint8_t MAPS[MAP_SIZE][4][4] = {
    MAP1, MAP2, MAP3, MAP4
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
#define ENEMY_SPRITE_FLAG 0x03

constexpr bool isEnemySpriteTexture(uint8_t tag) {
    uint8_t combined_tag = (SPRITE_TAG | ENEMY_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}

constexpr bool isPlayerSpriteTexture(uint8_t tag) {
    if (isEnemySpriteTexture(tag)) return false;
    uint8_t combined_tag = (SPRITE_TAG | PLAYER_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}

constexpr bool isBackgroundSpriteTexture(uint8_t tag) {
    if (isEnemySpriteTexture(tag)) return false;
    uint8_t combined_tag = (SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
    return (tag & combined_tag) == combined_tag;
}
constexpr bool isTextTexture(uint8_t tag) {
    if (isPlayerSpriteTexture(tag) || isEnemySpriteTexture(tag)) return false;
    return (tag & TEXT_TAG) == TEXT_TAG;
}
constexpr bool isImageTexture(uint8_t tag) {
    if (isBackgroundSpriteTexture(tag) ||
    isEnemySpriteTexture(tag)) return false;
    return (tag & IMAGE_TAG) == IMAGE_TAG;
}
constexpr bool isSpriteTexture(uint8_t tag) {
    // LOG_INFO("isSpriteTexture(%u)", tag);
    return (tag & SPRITE_TAG) == SPRITE_TAG ||
    isPlayerSpriteTexture(tag) ||
    isBackgroundSpriteTexture(tag) ||
    isEnemySpriteTexture(tag);
}

const bool Game::IsColliding(uint16_t x, uint16_t y) {
    uint16_t collide_y = (y + ((PLAYER_HEIGHT/4) * 3.5));
    uint16_t collide_x_right = (x + ((PLAYER_WIDTH/4) * 3.5));

    if (collide_x_right >= SCREEN_WIDTH || x <= 0) return false;

    uint8_t segment_x_left = (uint8_t)((x + PLAYER_WIDTH/4)/(SCREEN_WIDTH/4));
    uint8_t segment_x_right = (uint8_t)(collide_x_right/(SCREEN_WIDTH/4));
    uint8_t segment_y = (uint8_t)(collide_y/(SCREEN_HEIGHT/4));
    const bool is_colliding = MAPS[map_idx][segment_y][segment_x_left] == 1 ||
    MAPS[map_idx][segment_y][segment_x_right] == 1;
    if (is_colliding) LOG_INFO("Collision !");
    return is_colliding;
}

Game::Game() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Panic: SDL initialization failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        printf("Panic: Failed to initialize SDL_ttf, abort.\n");
        exit(EXIT_FAILURE);
    }

    _window = SDL_CreateWindow("Game", 100, 100, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!_window) {
        printf("panic: Window creation failed, abort.\n");
        exit(EXIT_FAILURE);
    }

    _renderer = SDL_CreateRenderer(_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
        },
        { .text_or_uri = "assets/enemy.png",
          .src_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
          .dst_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
          .color = {0,0,0,0},
          .tag = SPRITE_TAG | ENEMY_SPRITE_FLAG
        }
    };
    _scene_texture_locations.push_back(SCENE_1);
    _scene_texture_locations.push_back(SCENE_2);
    LOG_INFO("Game::SetTextureLocations() => Allocated %li locations !",
              _scene_texture_locations.size());
}

const bool Game::AfterMainMenu() {
    return _scene_stack_idx + 1 >= SCENE_STACK_MAX_SIZE;
}

void Game::AllocateScene(bool incrementStackIdx) {
    if (_scene_stack_idx + 1 < SCENE_STACK_MAX_SIZE) {
        if (incrementStackIdx) _scene_stack_idx++;
        scene_t scene;
        _scenes.push_back(scene);
        LOG_INFO("Game::AllocateScene() => Stack size: %li.", _scenes.size());
        LOG_INFO("Game::AllocateScene() => Stack index: %li.",
                  _scene_stack_idx);
        LOG_INFO("Game::AllocateScene() => Allocate %li textures for Scene: %i",
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

void Game::UpdateMap() {
    if (_player_y <= 0) {
        // Move north a map
        if (map_idx == 0) {
            map_idx = 3;
            _player_y = SCREEN_HEIGHT + _player_y;
            if (_player_y == SCREEN_HEIGHT) _player_y = SCREEN_HEIGHT - 1;
            LOG_INFO("Map index moved to: %i", map_idx);
        }
    } else if (_player_y >= SCREEN_HEIGHT) {
        // Move south a map
        if (map_idx == 3) {
            map_idx = 0;
            _player_y = SCREEN_HEIGHT - _player_y;
            if (_player_y == 0) _player_y = 1;
            LOG_INFO("Map index moved to: %i", map_idx);
        }
    } else if (_player_x <= 0) {
        // Move west a map
        if (map_idx == 2) {
            map_idx = 0;
            LOG_INFO("Map index moved to: %i", map_idx);
        } else if (map_idx == 0) {
            map_idx = 1;
            LOG_INFO("Map index moved to: %i", map_idx);
        }
        _player_x = SCREEN_WIDTH - abs(_player_x);
        if (_player_x == SCREEN_WIDTH) _player_x = SCREEN_WIDTH - 1;
    } else if (_player_x >= SCREEN_WIDTH) {
        // Move east a map
        if (map_idx == 0) {
            map_idx = 2;
            LOG_INFO("Map index moved to: %i", map_idx);
        } else if (map_idx == 1) {
            map_idx = 0;
            LOG_INFO("Map index moved to: %i", map_idx);
        }
        _player_x -= SCREEN_WIDTH;
        if (_player_x == 0) _player_x = 1;
    }
}

void AdjustBackgroundSprite(SDL_Rect* src_rect, uint8_t map_idx, uint8_t segment_y, uint8_t segment_x) {
    if (MAPS[map_idx][segment_y][segment_x] == 1) {
        src_rect->x = 49;
        src_rect->y = 48;
    } else {
        src_rect->x = 17;
        src_rect->y = 64;
    }
}

void Game::RenderScene() {
    float tick = SDL_GetTicks();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    else SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);
    for (uint8_t i = 0; i < _scenes[_scene_stack_idx].textures.size(); ++i) {
        SDL_Texture* texture = _scenes[_scene_stack_idx].textures[i];
        SDL_Rect src_rect = _scenes[_scene_stack_idx].texture_src_rects[i];
        SDL_Rect dst_rect = _scenes[_scene_stack_idx].texture_dst_rects[i];
        uint8_t tag = _scenes[_scene_stack_idx].tags[i];
        if (isTextTexture(tag)) {
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        }  else if (isBackgroundSpriteTexture(tag)) {
            dst_rect.w = 16;
            dst_rect.h = 16;

            int32_t camera_x = _player_x - PLAYER_BEGIN_X;
            int32_t camera_y = _player_y - PLAYER_BEGIN_Y;
            uint32_t begin_x = 0;
            if (camera_x >= 16) begin_x = camera_x - (camera_x % 16);
            uint16_t end_x = SCREEN_WIDTH;
            if (camera_x < 0) end_x = SCREEN_WIDTH - abs(camera_x);

            uint32_t begin_y = 0;
            if (camera_y >= 16) begin_y = camera_y - (camera_y % 16);

            if (_player_state == player_state_t::MOVING) LOG_INFO("Camera X: %i", camera_x);
            if (_player_state == player_state_t::MOVING) LOG_INFO("Camera Y: %i", camera_y);

            // Render the map the player is in
            for (uint16_t i = begin_x; i < end_x; i+=16) {
                uint16_t map_delta_x = i - begin_x;
                if (camera_x < 0) dst_rect.x = map_delta_x - camera_x;
                else dst_rect.x = map_delta_x - (camera_x % 16);
                uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=16) {
                    dst_rect.y = j - camera_y;
                    uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                    AdjustBackgroundSprite(&src_rect, map_idx, map_segment_y, map_segment_x);
                    SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                }
            }

            if (camera_y > 0) {
                // Map to the south
                if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the south");
                auto other_map_idx = 0;

                uint16_t begin_x = 0;
                if (camera_x > 16) {
                    begin_x = camera_x - (camera_x%16);
                }
                uint16_t end_x = SCREEN_WIDTH;
                if (camera_x < 16) {
                    end_x = SCREEN_WIDTH - camera_x + (camera_x % 16);
                }

                for (uint16_t i = begin_x; i < end_x; i+=16) {
                    dst_rect.x = i - camera_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y%16); j+=16) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        AdjustBackgroundSprite(&src_rect, other_map_idx, map_segment_y, map_segment_x);
                        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                    }
                }
            }
            if (camera_y < 0) {
                // Map to the north
                if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the north");
                auto other_map_idx = 3;

                uint16_t begin_x = 0;
                if (camera_x > 16) {
                    begin_x = camera_x - (camera_x%16);
                }
                uint16_t end_x = SCREEN_WIDTH;
                if (camera_x < 16) {
                    end_x = SCREEN_WIDTH - camera_x + (camera_x % 16);
                }

                uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y))%16);
                uint16_t map_camera_offset_y = ((SCREEN_HEIGHT - abs(camera_y))%16);

                for (uint16_t i = begin_x; i < end_x; i+=16) {
                    uint16_t delta_map_x = i - begin_x;
                    // dst_rect.x = i + (camera_x%16);
                    dst_rect.x = i - camera_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=16) {
                        uint16_t delta_map_y = j - begin_y;
                        dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y))%16) > 0 ? 16 : 0) + (abs(camera_y)%16);
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        AdjustBackgroundSprite(&src_rect, other_map_idx, map_segment_y, map_segment_x);
                        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                    }
                }
            }

            if (camera_x > 0) {
                // Map to the right.
                if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the right");
                auto other_map_idx = map_idx;
                if (map_idx == 0) other_map_idx = 2;
                else if (map_idx == 1) other_map_idx = 0;

                uint16_t begin_x = 0;
                uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

                uint16_t begin_y = 0;
                if (camera_y > 16) begin_y = camera_y - (camera_y % 16);
                uint16_t end_y = SCREEN_HEIGHT;
                if (camera_y < -16) end_y = SCREEN_HEIGHT + camera_y + 16 - (abs(camera_y) % 16);

                for (uint16_t i = begin_x; i < end_x; i+=16) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x + SCREEN_WIDTH;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < end_y; j+=16) {
                        uint16_t delta_map_y = j - begin_y;
                        if (camera_y > 0) {
                            dst_rect.y = delta_map_y -(abs(camera_y)%16);
                        } else {
                            dst_rect.y = delta_map_y + abs(camera_y);
                        }
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        AdjustBackgroundSprite(&src_rect, other_map_idx, map_segment_y, map_segment_x);
                        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                    }
                }
            }
            if (camera_x < 0) {
                // Map to the left
                if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the left");
                auto other_map_idx = map_idx;
                if (map_idx >= 2) other_map_idx = 0;
                else other_map_idx = 1;

                // We consider either "exactly" the right amount or 16 off.
                uint16_t begin_x = SCREEN_WIDTH + camera_x - (16 - abs(camera_x)%16);

                uint32_t begin_y = 0;
                if (camera_y > 16) begin_y = camera_y - (camera_y % 16);
                uint16_t end_y = SCREEN_HEIGHT;
                if (camera_y < -16) end_y = SCREEN_HEIGHT + camera_y + 16 - (abs(camera_y) % 16);

                for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=16) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = -16 + (abs(camera_x)%16) + delta_map_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));

                    for (uint16_t j = begin_y; j < end_y; j+=16) {
                        uint16_t delta_map_y = j - begin_y;
                        if (camera_y > 0) {
                            dst_rect.y = delta_map_y -(abs(camera_y)%16);
                        } else {
                            dst_rect.y = delta_map_y + abs(camera_y);
                        }
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        AdjustBackgroundSprite(&src_rect, other_map_idx, map_segment_y, map_segment_x);
                        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                    }
                }
            }

        } else if (isPlayerSpriteTexture(tag)) {
            // LOG_INFO("Player tag: %u", tag);
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

            dst_rect.x = PLAYER_BEGIN_X;
            dst_rect.y = PLAYER_BEGIN_Y;
            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
       }
       // else if (isEnemySpriteTexture(tag)) {
       //     // LOG_INFO("Enemy tag: %u", tag);
       //     dst_rect.x = 200;
       //     dst_rect.y = 200;
       //     SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
       // }
    }

    SDL_RenderPresent(_renderer);

    if (_scene_stack_idx == 0) {
        if (_scenes[_scene_stack_idx].textures.size() == 3) {
            // Remove the FPS texture.
            _scenes[_scene_stack_idx].textures.pop_back();
        }
    } else if (_scene_stack_idx == 1) {
        if (_scenes[_scene_stack_idx].textures.size() == 4) {
            // Remove the FPS texture.
            _scenes[_scene_stack_idx].textures.pop_back();
        }
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
