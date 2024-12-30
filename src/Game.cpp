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

uint64_t frame_count = 0;
uint64_t current_ticks = 0;

const bool Game::IsColliding(const int x, const int y) {
    uint8_t _map_idx = map_idx;

    int top_bounding_box = y;
    int bottom_bounding_box = y + PLAYER_HEIGHT;
    int left_bounding_box = x;
    int right_bounding_box = x + PLAYER_WIDTH;

    if (top_bounding_box < 0) {
        top_bounding_box += SCREEN_HEIGHT;
        _map_idx = GetNorthIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));
        return !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][left_bounding_box_quadrant]  == -1 &&
            MAPS[_map_idx-1][top_bounding_box_quadrant][right_bounding_box_quadrant] == -1
        );
    }

    if (bottom_bounding_box > SCREEN_HEIGHT) {
        bottom_bounding_box -= SCREEN_HEIGHT;
        _map_idx = GetSouthIdx(map_idx);

        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));

        return !(
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][right_bounding_box_quadrant] == -1 &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][left_bounding_box_quadrant] == -1
        );
    }

    if (right_bounding_box > SCREEN_WIDTH) {
        right_bounding_box -= SCREEN_WIDTH;
        _map_idx = GetEastIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        return !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][right_bounding_box_quadrant]    == -1 &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][right_bounding_box_quadrant] == -1
        );
    }

    if (left_bounding_box < 0) {
        left_bounding_box += SCREEN_WIDTH;
        _map_idx = GetWestIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));
        return !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][left_bounding_box_quadrant]    == -1 &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][left_bounding_box_quadrant] == -1
        );
    }

    uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
    uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
    uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
    uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));

    // We check the current map.
    return !(
        MAPS[_map_idx-1][top_bounding_box_quadrant][left_bounding_box_quadrant] == -1 &&
        MAPS[_map_idx-1][top_bounding_box_quadrant][right_bounding_box_quadrant] == -1 &&
        MAPS[_map_idx-1][bottom_bounding_box_quadrant][right_bounding_box_quadrant] == -1 &&
        MAPS[_map_idx-1][bottom_bounding_box_quadrant][left_bounding_box_quadrant] == -1
    );
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
        // { .text_or_uri = "assets/enemy.png",
        //   .src_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
        //   .dst_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
        //   .color = {0,0,0,0},
        //   .tag = SPRITE_TAG | ENEMY_SPRITE_FLAG
        // }
    };
    _common->AddScene(SCENE_1);
    _common->AddScene(SCENE_2);
    _common->SetInitialSceneTextureSize(SCENE_2.size());
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
    if (_player_y <= 0) {                    // Move north a map
        map_idx = GetNorthIdx(map_idx);
        _player_y = SCREEN_HEIGHT + _player_y;
        if (_player_y == SCREEN_HEIGHT) _player_y = SCREEN_HEIGHT - 1;
        LOG_INFO("Map index moved to: %i", map_idx);
    } else if (_player_y >= SCREEN_HEIGHT) { // Move south a map

        map_idx = GetSouthIdx(map_idx);
        _player_y = SCREEN_HEIGHT - _player_y;
        if (_player_y == 0) _player_y = 1;
        LOG_INFO("Map index moved to: %i", map_idx);
    } else if (_player_x <= 0) {             // Move west a map
        map_idx = GetWestIdx(map_idx);
        LOG_INFO("Map index moved to: %i", map_idx);
        _player_x = SCREEN_WIDTH - abs(_player_x);
        if (_player_x == SCREEN_WIDTH) _player_x = SCREEN_WIDTH - 1;
    } else if (_player_x >= SCREEN_WIDTH) {  // Move east a map
        map_idx = GetEastIdx(map_idx);
        LOG_INFO("Map index moved to: %i", map_idx);
        _player_x -= SCREEN_WIDTH;
        if (_player_x == 0) _player_x = 1;
    }
}

bool IsNotDefaultSprite(const uint8_t map_idx, const uint8_t segment_x, const uint8_t segment_y) {
    return MAPS[map_idx-1][segment_y][segment_x] != -1;
}

void AdjustBackgroundSprite(SDL_Rect* src_rect, uint8_t map_idx, uint8_t segment_y, uint8_t segment_x) {
    int v = MAPS[map_idx-1][segment_y][segment_x];
    if (v == -1)  {
        // Consider the default to be the grass texture.
        src_rect->x = 17;
        src_rect->y = 64;
    } else {
        uint8_t spritesheet_x = v / 16;
        uint8_t spritesheet_y = v % 16;
        src_rect->x = spritesheet_x * 16;
        src_rect->y = spritesheet_y * 16;
    }
}

void Game::RenderSprite(SDL_Rect& src_rect, SDL_Rect& dst_rect, const uint8_t map_idx, const uint8_t map_segment_y, const uint8_t map_segment_x, SDL_Texture& texture) {
    SDL_Renderer* _renderer = _common->GetRenderer();
    AdjustBackgroundSprite(&src_rect, map_idx, map_segment_y, map_segment_x);

    int v = MAPS[map_idx-1][map_segment_y][map_segment_x];
    if (v == -1) {
        SDL_RenderCopy(_renderer, &texture, &src_rect, &dst_rect);
        return;
    }
    // Render the grass first
    SDL_Rect srcRectGrass = {17, 64, src_rect.w, src_rect.h};
    SDL_RenderCopy(_renderer, &texture, &srcRectGrass, &dst_rect);

    // Render the actual sprite
    SDL_RenderCopy(_renderer, &texture, &src_rect, &dst_rect);
}

void Game::ResetAttackAnimation() {
    _attack_animation.active = false;
    _attack_animation.x = PLAYER_BEGIN_X;
    _attack_animation.runtime = 0;
}

void Game::ResetMoveAnimation() {
    _move_animation.remaining_ticks = 0;
}

void Game::DrawPlayerBoundingBox() {
   SDL_Renderer* _renderer = _common->GetRenderer();
   SDL_SetRenderDrawColor( _renderer, 0xFF, 0x00, 0x00, 0xFF );
   SDL_RenderDrawLine( _renderer, PLAYER_BEGIN_X, PLAYER_BEGIN_Y, PLAYER_BEGIN_X + PLAYER_WIDTH, PLAYER_BEGIN_Y);
   SDL_RenderDrawLine( _renderer, PLAYER_BEGIN_X, PLAYER_BEGIN_Y + PLAYER_HEIGHT, PLAYER_BEGIN_X + PLAYER_WIDTH, PLAYER_BEGIN_Y + PLAYER_HEIGHT);
   SDL_RenderDrawLine( _renderer, PLAYER_BEGIN_X, PLAYER_BEGIN_Y, PLAYER_BEGIN_X, PLAYER_BEGIN_Y + PLAYER_HEIGHT);
   SDL_RenderDrawLine( _renderer, PLAYER_BEGIN_X + PLAYER_WIDTH, PLAYER_BEGIN_Y, PLAYER_BEGIN_X + PLAYER_WIDTH, PLAYER_BEGIN_Y + PLAYER_HEIGHT);
}

// Function to draw a circle using the Midpoint Circle Algorithm
void Game::DrawCircle(int center_x, int center_y, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;
    auto _renderer = _common->GetRenderer();

    while (x >= y) {
        // Draw the eight octants
        SDL_RenderDrawPoint(_renderer, center_x + x, center_y + y);
        SDL_RenderDrawPoint(_renderer, center_x + y, center_y + x);
        SDL_RenderDrawPoint(_renderer, center_x - y, center_y + x);
        SDL_RenderDrawPoint(_renderer, center_x - x, center_y + y);
        SDL_RenderDrawPoint(_renderer, center_x - x, center_y - y);
        SDL_RenderDrawPoint(_renderer, center_x - y, center_y - x);
        SDL_RenderDrawPoint(_renderer, center_x + y, center_y - x);
        SDL_RenderDrawPoint(_renderer, center_x + x, center_y - y);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}

void Game::DrawSquare(const int begin_x, const int begin_y, const int w, const int h, const int bound_end_x, const int bound_end_y) {
   SDL_Renderer* _renderer = _common->GetRenderer();

   const int end_x = begin_x + w > bound_end_x ? bound_end_x : begin_x + w;
   const int end_y = begin_y + h > bound_end_y ? bound_end_y : begin_y + h;

   // Top line
   SDL_RenderDrawLine( _renderer, begin_x, begin_y, end_x, begin_y);

   // Left line
   SDL_RenderDrawLine( _renderer, begin_x, begin_y, begin_x, end_y);

   // Right line
   SDL_RenderDrawLine( _renderer, end_x, begin_y, end_x, end_y);

   // Bottom line
   SDL_RenderDrawLine( _renderer, begin_x, end_y, end_x, end_y);
}

void Game::RenderCurrentScene() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0XFF);
    else SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(_renderer);

    // Reset the draw color.
    SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF);

    // Camera X grows negatively when moving left in a given map.
    const int32_t camera_x = _player_x - PLAYER_BEGIN_X;
    // Camera Y grows positively when moving down in a given map.
    const int32_t camera_y = _player_y - PLAYER_BEGIN_Y;

    for (uint8_t i = 0; i < current_scene->textures.size(); ++i) {
        SDL_Texture* texture = current_scene->textures[i];
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        if (_common->isTextTexture(tag)) {
            // FIXME: This is a hack, we want to first draw the quad and then the text.
            // Menu quad. (optional)
            if (_game_state == game_state_t::PAUSE) {
                SDL_Rect fillRect = { SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.25, SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.4 };
                SDL_SetRenderDrawColor( _renderer, 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderFillRect( _renderer, &fillRect );
            }

            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        }  else if (_common->isBackgroundSpriteTexture(tag)) {
            // Render the spritesheet.

            // Scale the sprites by 3.125x
            dst_rect.w = 16 * SPRITE_SCALE_FACTOR;
            dst_rect.h = 16 * SPRITE_SCALE_FACTOR;

            uint32_t begin_x = 0;
            if (camera_x >= dst_rect.w) begin_x = camera_x - (camera_x % dst_rect.w);

            // Determine where the X ends depending on where the camera is.
            uint16_t end_x = SCREEN_WIDTH;
            if (camera_x < 0) end_x = SCREEN_WIDTH - abs(camera_x);

            uint32_t begin_y = 0;
            if (camera_y >= dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);

            // if (_player_state == player_state_t::MOVING) LOG_INFO("Camera X: %i", camera_x);
            // if (_player_state == player_state_t::MOVING) LOG_INFO("Camera Y: %i", camera_y);

            // Render the map the player is in
            for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                uint16_t map_delta_x = i - begin_x;
                if (camera_x < 0) dst_rect.x = map_delta_x - camera_x;        // If the camera is left of origin, shift sprite right.
                else dst_rect.x = map_delta_x - (camera_x % dst_rect.w);      // If the camera is right of origin, shift sprite left.
                uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                    uint16_t map_delta_y = j - begin_y;
                    if (camera_y < 0) dst_rect.y = map_delta_y - camera_y;    // If the camera is down of origin, shift sprite up.
                    else dst_rect.y = map_delta_y - (camera_y % dst_rect.h);  // If the camera is up of origin, shift sprite down.
                    uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                    RenderSprite(src_rect, dst_rect, map_idx, map_segment_y, map_segment_x, *texture);
                }
            }

            // Render the north east map
            if (camera_y < 0 && camera_x > 0) {
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
                        RenderSprite(src_rect, dst_rect, GetNorthEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the north west map
            if (camera_y < 0 && camera_x < 0) {
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
                        RenderSprite(src_rect, dst_rect, GetNorthWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the south east map
            if (camera_y > 0 && camera_x > 0) {
                uint16_t begin_x = 0;
                uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x + SCREEN_WIDTH;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        RenderSprite(src_rect, dst_rect, GetSouthEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the south west map
            if (camera_y > 0 && camera_x < 0) {
                // We consider either "exactly" the right amount or 16 off.
                uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.w - abs(camera_x) % dst_rect.w);

                for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
                        dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        RenderSprite(src_rect, dst_rect, GetSouthWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the south map
            if (camera_y > 0) {
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
                        RenderSprite(src_rect, dst_rect, GetSouthIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the north map
            if (camera_y < 0) {
                uint16_t begin_x = 0;
                if (camera_x > dst_rect.w) {
                    begin_x = camera_x - (camera_x % dst_rect.w);
                }
                uint16_t end_x = SCREEN_WIDTH;
                if (camera_x < dst_rect.w) {
                    end_x = SCREEN_WIDTH - camera_x + (camera_x % dst_rect.w);
                }

                // Camera points down.
                uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h);

                for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
                    uint16_t delta_map_x = i - begin_x;
                    dst_rect.x = i - camera_x;
                    uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
                    for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
                        uint16_t delta_map_y = j - begin_y;
                        dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
                        uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
                        RenderSprite(src_rect, dst_rect, GetNorthIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the east map
            if (camera_x > 0) {
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
                        RenderSprite(src_rect, dst_rect, GetEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }

            // Render the west map
            if (camera_x < 0) {
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
                        RenderSprite(src_rect, dst_rect, GetWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
                    }
                }
            }
        } else if (_common->isPlayerSpriteTexture(tag)) {
            if (_attack_animation.active && _attack_animation.runtime > 0) {
                LOG_INFO("Attack animation x: %i", _attack_animation.x);
                _attack_animation.runtime -= 1;
                src_rect.y = PLAYER_HEIGHT;
                if (_attack_animation.runtime >= ATTACK_ANIMATION_FRAMES/2) src_rect.x = PLAYER_WIDTH;
                else src_rect.x = PLAYER_WIDTH*2;

                float dt = ATTACK_ANIMATION_FRAMES+1 - _attack_animation.runtime / ATTACK_ANIMATION_FRAMES;
                _attack_animation.x += 0.125 * dt;

                dst_rect.x = _attack_animation.x;
            } else {
                dst_rect.x = PLAYER_BEGIN_X;
                dst_rect.y = PLAYER_BEGIN_Y;
                if (_attack_animation.active) ResetAttackAnimation();
                if (_player_state == player_state_t::MOVING) {
                    if (_move_animation.remaining_ticks == 0 || _move_animation.direction != _player_direction) {
                        _move_animation.remaining_ticks = MOVE_ANIM_TICKS;
                        _move_animation.direction = _player_direction;
                    } else {
                        TickPlayerMove();
                    }
                    if (_move_animation.direction == player_direction_t::UP) {
                        if (_move_animation.remaining_ticks <= MOVE_ANIM_TICKS/2) src_rect.x = PLAYER_WIDTH*3;
                        else src_rect.x = PLAYER_WIDTH*4;
                    } else {
                        if (_move_animation.remaining_ticks <= MOVE_ANIM_TICKS/2) src_rect.x = 0;
                        else src_rect.x = PLAYER_WIDTH * 2;
                    }
                    src_rect.y = 0;
                    current_scene->texture_src_rects[i] = src_rect;
                } else if (_player_state == player_state_t::STOPPED) {
                    src_rect.x = PLAYER_WIDTH;
                    src_rect.y = 0;
                }
            }

            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
       }
   }

   // Temporarily draw the bounding box.
   // DrawPlayerBoundingBox();

   // Draw the minimap.
   int minimap_begin_x = SCREEN_WIDTH * 0.85;
   int minimap_end_x = SCREEN_WIDTH * 0.95;
   int minimap_begin_y = SCREEN_HEIGHT * 0.05 * (4/3);
   int minimap_end_y = SCREEN_HEIGHT * 0.15 * (4/3);
   SDL_SetRenderDrawColor( _renderer, 0xFF, 0xFF, 0xFF, 0xAA );
   SDL_RenderDrawLine( _renderer, minimap_begin_x, minimap_begin_y, minimap_end_x, minimap_begin_y);
   SDL_RenderDrawLine( _renderer, minimap_begin_x, minimap_begin_y, minimap_begin_x, minimap_end_y);
   SDL_RenderDrawLine( _renderer, minimap_end_x, minimap_begin_y, minimap_end_x, minimap_end_y);
   SDL_RenderDrawLine( _renderer, minimap_begin_x, minimap_end_y, minimap_end_x, minimap_end_y);

   // Represents the player in the minimap.
   DrawCircle((minimap_begin_x + minimap_end_x) / 2, (minimap_begin_y + minimap_end_y) / 2, 2);

   // Draw non-default sprites as squares inside minimap.
   const uint8_t SPRITE_SIZE = (16 * SPRITE_SCALE_FACTOR);
   for (uint16_t i = 0; i < SCREEN_WIDTH; i+=SCREEN_WIDTH/4) {
       const uint16_t dx = i - camera_x;
       uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
       for (uint16_t j = 0; j < SCREEN_HEIGHT; j+=SCREEN_HEIGHT/4) {
           uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
           const uint16_t dy = j - camera_y;
           if (IsNotDefaultSprite(map_idx, map_segment_x, map_segment_y)) {
               const int minimap_offset_x = (dx * 0.1);
               const int minimap_offset_y = (dy * 0.1);
               const int begin_x = minimap_begin_x + minimap_offset_x;
               const int begin_y = minimap_begin_y + minimap_offset_y;
               if (begin_x > minimap_end_x > 0.1) continue;
               if (begin_y > minimap_end_y) continue;
               DrawSquare(begin_x, begin_y, SCREEN_WIDTH*0.1 / 4, SCREEN_HEIGHT*0.1 / 4 *(4/3), minimap_end_x, minimap_end_y);
           }
       }
   }

   // Reset the color
   SDL_SetRenderDrawColor( _renderer, 0x00, 0x00, 0x00, 0xFF );

   SDL_RenderPresent(_renderer);

   if (_scene_stack_idx == 0) return;
   if (_scene_stack_idx == 1) {
       // Cleanup temporary textures.
       while(current_scene->textures.size() > _common->GetInitialSceneTextureSize()) {
           SDL_DestroyTexture(current_scene->textures[current_scene->textures.size() - 1]);
           current_scene->textures.pop_back();
           current_scene->texture_src_rects.pop_back();
           current_scene->texture_dst_rects.pop_back();
           current_scene->tags.pop_back();
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

       if (_game_state == game_state_t::PAUSE) {
           //Render main menu (optionally)
           gametexture_t menu_text_texture = {
               .text_or_uri = "SAVE",
               .src_rect = {0, 0, 0, 0},
               .dst_rect = {SCREEN_WIDTH*0.375 + (SCREEN_WIDTH*0.1), SCREEN_HEIGHT*0.25 + (SCREEN_HEIGHT*0.1), 0, 0},
               .color = {255,255,255,255},
               .font_size = Common::FONT_SIZE::MEDIUM,
               .tag = TEXT_TAG
           };
           _common->LoadTexture(_scene_stack_idx, menu_text_texture);
       }

   }
}

void Game::HandleSpaceKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (!AfterMainMenu()) _common->AllocateScene(true);
    else {
        SetPlayerState(player_state_t::ATTACK);
        SetAttackAnimation(ATTACK_ANIMATION_FRAMES, true);
    }
}

void Game::HandleUpKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (IsColliding(GetPlayerX(), GetPlayerY() - STEP_SIZE)) {
        return;
    }
    SetPlayerY(GetPlayerY() - STEP_SIZE);
    SetPlayerState(player_state_t::MOVING);
    SetPlayerDirection(player_direction_t::UP);
    UpdateMap();
}

void Game::HandleDownKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (IsColliding(GetPlayerX(), GetPlayerY() + STEP_SIZE)) {
        return;
    }
    SetPlayerY(GetPlayerY() + STEP_SIZE);
    SetPlayerState(player_state_t::MOVING);
    SetPlayerDirection(player_direction_t::DOWN);
    UpdateMap();
}

void Game::HandleLeftKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (IsColliding(GetPlayerX() - STEP_SIZE, GetPlayerY())) return;
    SetPlayerX(GetPlayerX() - STEP_SIZE);
    SetPlayerState(player_state_t::MOVING);
    SetPlayerDirection(player_direction_t::LEFT);
    UpdateMap();
}

void Game::HandleRightKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (IsColliding(GetPlayerX() + STEP_SIZE, GetPlayerY())) return;
    SetPlayerX(GetPlayerX() + STEP_SIZE);
    SetPlayerState(player_state_t::MOVING);
    SetPlayerDirection(player_direction_t::RIGHT);
    UpdateMap();
}

void Game::HandleEscKey() {
    LOG_INFO("Escape key pressed");
    if (_game_state == game_state_t::PAUSE) _game_state = game_state_t::PLAY;
    else _game_state = game_state_t::PAUSE;
}

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
                    game->HandleSpaceKey();
                } else if (keysym.sym == SDLK_UP) {
                    game->HandleUpKey();
                    break;
                } else if (keysym.sym == SDLK_DOWN) {
                    game->HandleDownKey();
                    break;
                } else if (keysym.sym == SDLK_LEFT) {
                    game->HandleLeftKey();
                    break;
                } else if (keysym.sym == SDLK_RIGHT) {
                    game->HandleRightKey();
                    break;
                } else if (keysym.sym == SDLK_ESCAPE) {
                    game->HandleEscKey();
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

// TODO: Draw minimap.
// TODO: Menu on ESC.
// TODO: Save game.
// TODO: First use player sprite as NPC to test out dialog.
