#include "Game.h"
#include <string>
#include <cstdlib>
#include "Map.h"

// Center
#define C1 {\
    {1, -1, -1, 1},\
    {-1, -1, 2, -1},\
    {-1, -1, -1, -1},\
    {1, 1, 1, 1}\
}

// Left of center
#define L1 {\
    {1, 1, 1, 1},\
    {1, -1, 2, -1},\
    {1, -1, 1, -1},\
    {1, 1, 1, 1}\
}

// Right of center
#define R1 {\
    {1, 1, 1, 1},\
    {-1, -1, 2, 1},\
    {-1, -1, 1, 1},\
    {1, 1, 1, 1}\
}

#define C2 {\
    {1, -1, -1, 1},\
    {1, -1, 2, -1},\
    {1, -1, 1, -1},\
    {1, -1, -1, 1}\
}

#define E {\
    {1, 1, 1, 1},\
    {1, 1, 1, 1},\
    {1, 1, 1, 1},\
    {1, 1, 1, 1}\
}

#define C3 {\
    {1, 1, 1, 1},\
    {1, -1, 2, 1},\
    {1, -1, 1, 1},\
    {1, -1, -1, 1}\
}

// Current player map.
int MAP[4][4] = MAP0;
// uint8_t MAP[4][4] = C1;

// Player starts in the first center.
uint8_t map_idx = 2;

const uint8_t MAP_SIZE = 9;
// constexpr uint8_t MAPS[MAP_SIZE][4][4] = {
//     L1, C1, R1, E, C2, E, R1, C3, E
// };

constexpr int MAPS[MAP_SIZE][4][4] = {
    L1, MAP0, R1, E, C2, E, R1, C3, E
};

const bool Game::IsColliding(uint16_t x, uint16_t y) {
    uint16_t collide_y = (y + ((PLAYER_HEIGHT/4) * 3.5));
    uint16_t collide_x_right = (x + ((PLAYER_WIDTH/4) * 3.5));

    if (collide_x_right >= SCREEN_WIDTH || x <= 0) return false;

    uint8_t segment_x_left = (uint8_t)((x + PLAYER_WIDTH/4)/(SCREEN_WIDTH/4));
    uint8_t segment_x_right = (uint8_t)(collide_x_right/(SCREEN_WIDTH/4));
    uint8_t segment_y = (uint8_t)(collide_y/(SCREEN_HEIGHT/4));
    if (segment_y >= 4) return false;
    const bool is_colliding = MAPS[map_idx-1][segment_y][segment_x_left] >= 64 ||
                              MAPS[map_idx-1][segment_y][segment_x_right] >= 64;
    if (is_colliding) {
        LOG_INFO("Collision on map_idx=%i, player x: %i, player y: %i !!!", map_idx, _player_x, _player_y);
        LOG_INFO("In segment_y: %i, segment_x_left: %i, segment_x_right: %i", segment_y, segment_x_left, segment_x_right);
    }
    return is_colliding;
}

Game::Game(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    _SetTextureLocations();
    common_ptr->AllocateScene(false);
}

void Game::_SetTextureLocations() {
    const float SCREEN_CENTER = SCREEN_WIDTH/2.5 - 20.0f;
    const vector<gametexture_t> SCENE_1 = {
        { .text_or_uri = "Forged Memories",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {(uint32_t)SCREEN_CENTER, 50, 0, 0},
          .color = {255,255,255,255},
          .font_size = Common::FONT_SIZE::MEDIUM,
          .tag = TEXT_TAG
        },
        { .text_or_uri = "<SPC> to play",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {(uint32_t)SCREEN_CENTER, SCREEN_HEIGHT/2 - 1, 0, 0},
          .color = {255,255,255,255},
          .font_size = Common::FONT_SIZE::MEDIUM,
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
        { .text_or_uri = "assets/player2.png",
          .src_rect = {PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT},
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
    _common->AddScene(SCENE_1);
    _common->AddScene(SCENE_2);
}

const bool Game::AfterMainMenu() {
    return _common->GetSceneStackIdx() + 1 >= 2;
}

Game::~Game() {}

uint8_t Game::GetCenterIdx(uint8_t idx) {
    uint8_t next_idx = (uint8_t)(idx / 4) * 4 + 2;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetCenterIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetNorthIdx(uint8_t idx) {
    // If no next north, return default
    uint8_t next_idx = idx + 3;
    if (next_idx > MAP_SIZE) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetNorthIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthIdx(uint8_t idx) {
    uint8_t next_idx = idx - 3;
    if ((idx - 3) < 1) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetSouthIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetWestIdx(uint8_t idx) {
    uint8_t next_idx = idx - 1;
    if (next_idx < 1) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetEastIdx(uint8_t idx) {
    // For now, consider a row as size 3.
    uint8_t next_idx = idx + 1;
    if (next_idx > MAP_SIZE) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetEastIdx(idx=%i) => %i", idx, next_idx);

    return next_idx;
}

uint8_t Game::GetNorthEastIdx(uint8_t idx) {
    uint8_t next_idx = idx + 4;
    if (idx + 4 > MAP_SIZE) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetNorthEastIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetNorthWestIdx(uint8_t idx) {
    uint8_t next_idx = idx + 2;
    if (idx + 2 > MAP_SIZE) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetNorthWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthEastIdx(uint8_t idx) {
    uint8_t next_idx = idx - 2;
    if (idx - 2 < 1) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetSouthEastIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthWestIdx(uint8_t idx) {
    uint8_t next_idx = idx - 4;
    if (idx - 4 < 1) next_idx = 0;
    // if (_player_state == player_state_t::MOVING) LOG_INFO("GetSouthWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

void Game::UpdateMap() {
    if (_player_y <= 0) {
        // Move north a map
        map_idx = GetNorthIdx(map_idx);
        _player_y = SCREEN_HEIGHT + _player_y;
        if (_player_y == SCREEN_HEIGHT) _player_y = SCREEN_HEIGHT - 1;
        LOG_INFO("Map index moved to: %i", map_idx);
    } else if (_player_y >= SCREEN_HEIGHT) {
        // Move south a map
        map_idx = GetSouthIdx(map_idx);
        _player_y = SCREEN_HEIGHT - _player_y;
        if (_player_y == 0) _player_y = 1;
        LOG_INFO("Map index moved to: %i", map_idx);
    } else if (_player_x <= 0) {
        // Move west a map
        map_idx = GetWestIdx(map_idx);
        LOG_INFO("Map index moved to: %i", map_idx);
        _player_x = SCREEN_WIDTH - abs(_player_x);
        if (_player_x == SCREEN_WIDTH) _player_x = SCREEN_WIDTH - 1;
    } else if (_player_x >= SCREEN_WIDTH) {
        // Move east a map
        map_idx = GetEastIdx(map_idx);
        LOG_INFO("Map index moved to: %i", map_idx);
        _player_x -= SCREEN_WIDTH;
        if (_player_x == 0) _player_x = 1;
    }
}

void AdjustBackgroundSprite(SDL_Rect* src_rect, uint8_t map_idx, uint8_t segment_y, uint8_t segment_x) {
    int v = MAPS[map_idx-1][segment_y][segment_x];
    if (v == -1)  {
        src_rect->x = 17;
        src_rect->y = 64;
        // src_rect->x = 49;
        // src_rect->y = 48;
    } else {
        // src_rect->x = 17;
        // src_rect->y = 64;
        uint8_t spritesheet_x = v / 16;
        uint8_t spritesheet_y = v % 16;
        // // LOG_INFO("Spritesheet X: %i", spritesheet_x);
        // // LOG_INFO("Spritesheet Y: %i", spritesheet_y);
        src_rect->x = spritesheet_x * 16;
        src_rect->y = spritesheet_y * 16;
    }
}

void Game::RenderSprite(SDL_Rect& src_rect, SDL_Rect& dst_rect, const uint8_t map_idx, const uint8_t other_map_idx, const uint8_t map_segment_y, const uint8_t map_segment_x, SDL_Texture& texture) {
    SDL_Renderer* _renderer = _common->GetRenderer();
    AdjustBackgroundSprite(&src_rect, other_map_idx, map_segment_y, map_segment_x);
    const SDL_Rect sr = {src_rect.x, src_rect.y, src_rect.w, src_rect.h};
    const SDL_Rect dr = {dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h};
    int v = MAPS[map_idx-1][map_segment_y][map_segment_x];
    if (v <= 16) {
        SDL_Rect srcRectGrass = {17, 64, src_rect.w, src_rect.h};
        SDL_Rect dstRectGrass = {dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h};
        SDL_RenderCopy(_renderer, &texture, &srcRectGrass, &dstRectGrass);

        SDL_SetTextureAlphaMod(&texture, 0xFF);
        SDL_Rect dstRectTree = {dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h};
        SDL_RenderCopy(_renderer, &texture, &sr, &dstRectTree);
    } else {
        SDL_RenderCopy(_renderer, &texture, &sr, &dr);
    }
}

void Game::RenderCurrentScene() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    else SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(_renderer);

    for (uint8_t i = 0; i < current_scene->textures.size(); ++i) {
        SDL_Texture* texture = current_scene->textures[i];
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        if (_common->isTextTexture(tag)) {
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        }  else if (_common->isBackgroundSpriteTexture(tag)) {
            dst_rect.w = 50;
            dst_rect.h = 50;

            int32_t camera_x = _player_x - PLAYER_BEGIN_X;
            int32_t camera_y = _player_y - PLAYER_BEGIN_Y;

            uint32_t begin_x = 0;
            if (camera_x >= dst_rect.w) begin_x = camera_x - (camera_x % dst_rect.w);
            uint16_t end_x = SCREEN_WIDTH;
            if (camera_x < 0) end_x = SCREEN_WIDTH - abs(camera_x);
            uint32_t begin_y = 0;
            if (camera_y >= dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);

            // if (_player_state == player_state_t::MOVING) LOG_INFO("Camera X: %i", camera_x);
            // if (_player_state == player_state_t::MOVING) LOG_INFO("Camera Y: %i", camera_y);

            // Render the map the player is in
            for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                uint16_t map_delta_x = i - begin_x;
                if (camera_x < 0) dst_rect.x = map_delta_x - camera_x;
                else dst_rect.x = map_delta_x - (camera_x % dst_rect.w);
                uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                    dst_rect.y = j - camera_y;
                    uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                    RenderSprite(src_rect, dst_rect, map_idx, NULL, map_segment_y, map_segment_x, *texture);
                }
            }

            if (camera_y < 0 && camera_x > 0) {
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the north east");
                auto other_map_idx = GetNorthEastIdx(map_idx);

                uint16_t begin_x = 0;
                uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

                uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y))%dst_rect.h);

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x + SCREEN_WIDTH;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));

                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

            if (camera_y < 0 && camera_x < 0) {
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the north west");
                auto other_map_idx = GetNorthWestIdx(map_idx);

                // We consider either "exactly" the right amount or 16 off.
                uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.w - abs(camera_x) % dst_rect.w);
                uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h);

                for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

            if (camera_y > 0 && camera_x > 0) {
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the south east");
                auto other_map_idx = GetSouthEastIdx(map_idx);

                uint16_t begin_x = 0;
                uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x + SCREEN_WIDTH;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

            if (camera_y > 0 && camera_x < 0) {
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the south west");
                auto other_map_idx = GetSouthWestIdx(map_idx);

                // We consider either "exactly" the right amount or 16 off.
                uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.w - abs(camera_x) % dst_rect.w);

                for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

            if (camera_y > 0) {
                // Map to the south
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the south");
                auto other_map_idx = GetSouthIdx(map_idx);

                uint16_t begin_x = 0;
                if (camera_x > dst_rect.w) {
                    begin_x = camera_x - (camera_x % dst_rect.w);
                }
                uint16_t end_x = SCREEN_WIDTH;
                if (camera_x < dst_rect.w) {
                    end_x = SCREEN_WIDTH - camera_x + (camera_x % dst_rect.w);
                }

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    dst_rect.x = i - camera_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }
            if (camera_y < 0) {
                // Map to the north
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the north, player is in %i", map_idx);
                auto other_map_idx = GetNorthIdx(map_idx);

                uint16_t begin_x = 0;
                if (camera_x > dst_rect.w) {
                    begin_x = camera_x - (camera_x % dst_rect.w);
                }
                uint16_t end_x = SCREEN_WIDTH;
                if (camera_x < dst_rect.w) {
                    end_x = SCREEN_WIDTH - camera_x + (camera_x % dst_rect.w);
                }

                uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h);

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

            if (camera_x > 0) {
                // Map to the right.
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the east");
                auto other_map_idx = GetEastIdx(map_idx);

                uint16_t begin_x = 0;
                uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

                uint16_t begin_y = 0;
                if (camera_y > dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);
                uint16_t end_y = SCREEN_HEIGHT;
                if (camera_y < - dst_rect.h) end_y = SCREEN_HEIGHT + camera_y + dst_rect.h - (abs(camera_y) % dst_rect.h);

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x + SCREEN_WIDTH;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < end_y; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        if (camera_y > 0) {
                            dst_rect.y = delta_map_y -(abs(camera_y) % dst_rect.h);
                        } else {
                            dst_rect.y = delta_map_y + abs(camera_y);
                        }
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }
            if (camera_x < 0) {
                // if (_player_state == player_state_t::MOVING) LOG_INFO("Render map to the west");
                auto other_map_idx = GetWestIdx(map_idx);

                // We consider either "exactly" the right amount or 16 off.
                uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.h - abs(camera_x) % dst_rect.h);

                uint32_t begin_y = 0;
                if (camera_y > dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);
                uint16_t end_y = SCREEN_HEIGHT;
                if (camera_y < - dst_rect.h) end_y = SCREEN_HEIGHT + camera_y + dst_rect.h - (abs(camera_y) % dst_rect.h);

                for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));

                    for (uint16_t j = begin_y; j < end_y; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        if (camera_y > 0) {
                            dst_rect.y = delta_map_y -(abs(camera_y)%dst_rect.h);
                        } else {
                            dst_rect.y = delta_map_y + abs(camera_y);
                        }
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        if (other_map_idx == 0) {
                            // Render default sprite
                            src_rect.x = 49;
                            src_rect.y = 48;
                            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                        } else {
                            RenderSprite(src_rect, dst_rect, map_idx, other_map_idx, map_segment_y, map_segment_x, *texture);
                        }
                    }
                }
            }

        } else if (_common->isPlayerSpriteTexture(tag)) {
            // LOG_INFO("Player tag: %u", tag);
            if (_player_state == player_state_t::MOVING) {
                if (_player_direction == player_direction_t::UP) {
                    src_rect.x = PLAYER_WIDTH * 3;
                } else {
                    src_rect.x = src_rect.x == 0 ? PLAYER_WIDTH * 2 : 0;
                }
                src_rect.y = 0;
                current_scene->texture_src_rects[i] = src_rect;
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

    if (_scene_stack_idx == 0) return;
    if (_scene_stack_idx == 1) {
        if (current_scene->textures.size() >= 4) {
            // Remove the FPS texture.
            SDL_DestroyTexture(current_scene->textures[3]);
            current_scene->textures.pop_back();
        }
        gametexture_t fps_texture = {
            .text_or_uri = "FPS: " + std::to_string(_fps),
            .src_rect = {0, 0, 0, 0},
            .dst_rect = {20, 20, 0, 0},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::LARGE,
            .tag = TEXT_TAG
        };

        _common->LoadTexture(_scene_stack_idx, fps_texture);
    }

}

#define STEP_SIZE 5

void handleSpaceKey(std::shared_ptr<Common>& common, std::unique_ptr<Game>& game) {
    if (!game->AfterMainMenu()) common->AllocateScene(true);
    else {
        game->SetPlayerState(player_state_t::ATTACK);
    }
}

void handleUpKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX(),
        game->GetPlayerY() - STEP_SIZE)) return;
    int32_t next_y = game->GetPlayerY() - STEP_SIZE;
    game->SetPlayerY(next_y);
    // LOG_INFO("Player y: %i", next_y);
    game->SetPlayerState(player_state_t::MOVING);
    game->SetPlayerDirection(player_direction_t::UP);
    game->UpdateMap();
}

void handleDownKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX(),
        game->GetPlayerY() + STEP_SIZE)) return;
    int32_t next_y = game->GetPlayerY() + STEP_SIZE;
    game->SetPlayerY(next_y);
    // LOG_INFO("Player y: %i", next_y);
    game->SetPlayerState(player_state_t::MOVING);
    game->SetPlayerDirection(player_direction_t::DOWN);
    game->UpdateMap();
}

void handleLeftKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX() - STEP_SIZE,
        game->GetPlayerY())) return;
    int32_t next_x = game->GetPlayerX() - STEP_SIZE;
    game->SetPlayerX(next_x);
    // LOG_INFO("Player x: %i", next_x);
    game->SetPlayerState(player_state_t::MOVING);
    game->SetPlayerDirection(player_direction_t::LEFT);
    game->UpdateMap();
}

void handleRightKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX() + STEP_SIZE,
        game->GetPlayerY())) return;
    int32_t next_x = game->GetPlayerX() + STEP_SIZE;
    game->SetPlayerX(next_x);
    // LOG_INFO("Player x: %i", next_x);
    game->SetPlayerState(player_state_t::MOVING);
    game->SetPlayerDirection(player_direction_t::RIGHT);
    game->UpdateMap();
}

uint64_t frame_count = 0;
uint64_t current_ticks = 0;

int main() {
    std::shared_ptr<Common> common_ptr = std::make_shared<Common>(std::move(std::string("Forged Memories")));
    std::unique_ptr<Game> game = std::make_unique<Game>(common_ptr);

    int quit_app = 0;
    int dt, start_ticks, end_ticks = 0;

    while (quit_app == 0){
        SDL_Event EventHandler;
        while (SDL_PollEvent(&EventHandler) != 0) {
            if (EventHandler.type == SDL_QUIT) quit_app = 1;
            else if (EventHandler.type == SDL_KEYDOWN) {
                SDL_KeyboardEvent key = EventHandler.key;
                SDL_Keysym keysym = key.keysym;
                if (keysym.sym == SDLK_SPACE) {
                    handleSpaceKey(common_ptr, game);
                } else if (keysym.sym == SDLK_UP) {
                    handleUpKey(game);
                    break;
                } else if (keysym.sym == SDLK_DOWN) {
                    handleDownKey(game);
                    break;
                } else if (keysym.sym == SDLK_LEFT) {
                    handleLeftKey(game);
                    break;
                } else if (keysym.sym == SDLK_RIGHT) {
                    handleRightKey(game);
                    break;
                }
            }
            else if (EventHandler.type == SDL_KEYUP) {
                game->SetPlayerState(player_state_t::STOPPED);
            }
        }
        start_ticks = SDL_GetTicks();
        current_ticks = SDL_GetTicks();
        dt = start_ticks - end_ticks;
        if (dt > 1000 / 60.0) {
            game->RenderCurrentScene();
            if (current_ticks > 1000) {
                uint8_t fps = frame_count / (current_ticks/1000);
                game->set_fps(fps);
            }
            frame_count++;
            end_ticks = start_ticks;
        }
    }
    return 0;
}
