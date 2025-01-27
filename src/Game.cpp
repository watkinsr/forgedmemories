#include "Game.h"
#include <string>
#include <cstdlib>
#include <cassert>
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

Game::Game(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    assert(PLAYER_BEGIN_X % STEP_SIZE == 0);
    assert(PLAYER_BEGIN_Y % STEP_SIZE == 0);
    LOG_nodt(1, "INFO", "<Player begin_x=%zu, begin_y=%zu>\n", PLAYER_BEGIN_X, PLAYER_BEGIN_Y);
    _SetTextureLocations();
    common_ptr->AllocateScene(false);
    FillBackBufferInitial();
}

Game::~Game() {}

void Game::_SetTextureLocations() {
    const float SCREEN_CENTER = SCREEN_WIDTH/2.5 - 20.0f;
    const int SPRITESHEET_WIDTH = 128*SPRITE_SCALE_FACTOR;
    const int SPRITESHEET_HEIGHT = 128*SPRITE_SCALE_FACTOR;
    const int PLAYER_UPSCALE_WIDTH = 244*PLAYER_SCALE_FACTOR;
    const int PLAYER_UPSCALE_HEIGHT = 144*PLAYER_SCALE_FACTOR;
    const vector<gametexture_t> SCENE_1 = {
        { .text_or_uri = "Forged Memories",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {(uint32_t)SCREEN_CENTER, 50, 0, 0},
          .color = {255,255,255,255},
          .font_size = FONT_SIZE::MEDIUM,
          .tag = TEXT_TAG
        },
        { .text_or_uri = "<SPC> to play",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {(uint32_t)SCREEN_CENTER, SCREEN_HEIGHT/2 - 1, 0, 0},
          .color = {255,255,255,255},
          .font_size = FONT_SIZE::MEDIUM,
          .tag = TEXT_TAG
        }
    };
    const vector<gametexture_t> SCENE_2 = {
        { .text_or_uri = "assets/bg/Berry Garden.png",
          .src_rect = {0, 0, SPRITE_WIDTH, SPRITE_HEIGHT},
          .dst_rect = {0, 0, 64, 128},
          .color = {0,0,0,0},
          .tag = SPRITE_TAG | BACKGROUND_SPRITE_FLAG,
          .upscale = { SPRITESHEET_WIDTH, SPRITESHEET_HEIGHT }
        },
        { .text_or_uri = "assets/player2.png",
          .src_rect = {PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT},
          .dst_rect = {_player_x, _player_y, PLAYER_WIDTH, PLAYER_HEIGHT},
          .color = {0,0,0,0},
          .tag = SPRITE_TAG | PLAYER_SPRITE_FLAG,
          .upscale = { PLAYER_UPSCALE_WIDTH, PLAYER_UPSCALE_HEIGHT }
        }
    };
    _common->AddScene(SCENE_1);
    _common->AddScene(SCENE_2);
    _common->SetInitialSceneTextureSize(SCENE_2.size());
}

std::string btos(bool b) { return b ? "yes" : "no"; }

bool Game::IsColliding(const int x, const int y) {
    uint8_t _map_idx = map_idx;

    int top_bounding_box = y;
    int bottom_bounding_box = y + PLAYER_HEIGHT;
    int left_bounding_box = x;
    int right_bounding_box = x + PLAYER_WIDTH;

    bool didCollide = false;

    // FIXME: In practice this can be multiple spritesheet values.
    //        Which means we need a way of encoding whether the target
    //        can be moved through or not. For now, we just say, ok if it's not grass, collision.
    int TRANSPARENT_TARGET = -1;

    if (top_bounding_box < 0) {
        top_bounding_box += SCREEN_HEIGHT;
        _map_idx = GetNorthIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));
        didCollide = !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][left_bounding_box_quadrant]  == TRANSPARENT_TARGET &&
            MAPS[_map_idx-1][top_bounding_box_quadrant][right_bounding_box_quadrant] == TRANSPARENT_TARGET
        );
        if (didCollide) {
            LOG(1, "INFO", "Collision detected !\n");
        }
    }
    else if (bottom_bounding_box > SCREEN_HEIGHT) {
        bottom_bounding_box -= SCREEN_HEIGHT;
        _map_idx = GetSouthIdx(map_idx);

        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));

        didCollide = !(
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][right_bounding_box_quadrant] == TRANSPARENT_TARGET &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][left_bounding_box_quadrant] == TRANSPARENT_TARGET
        );
        if (didCollide) {
            LOG(1, "INFO", "Collision detected !\n");
        }
    }
    else if (right_bounding_box > SCREEN_WIDTH) {
        right_bounding_box -= SCREEN_WIDTH;
        _map_idx = GetEastIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t right_bounding_box_quadrant = (right_bounding_box/(SCREEN_WIDTH/4));
        didCollide = !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][right_bounding_box_quadrant]    == TRANSPARENT_TARGET &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][right_bounding_box_quadrant] == TRANSPARENT_TARGET
        );
        if (didCollide) {
            LOG(1, "INFO", "Collision detected !\n");
        }
    }
    else if (left_bounding_box < 0) {
        left_bounding_box += SCREEN_WIDTH;
        _map_idx = GetWestIdx(map_idx);
        uint8_t top_bounding_box_quadrant = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t bottom_bounding_box_quadrant = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t left_bounding_box_quadrant = (left_bounding_box/(SCREEN_WIDTH/4));
        didCollide = !(
            MAPS[_map_idx-1][top_bounding_box_quadrant][left_bounding_box_quadrant]    == TRANSPARENT_TARGET &&
            MAPS[_map_idx-1][bottom_bounding_box_quadrant][left_bounding_box_quadrant] == TRANSPARENT_TARGET
        );
        if (didCollide) {
            LOG(1, "INFO", "Collision detected !\n");
        }
    } else {
        uint8_t tq = (top_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t bq = (bottom_bounding_box/(SCREEN_HEIGHT/4));
        uint8_t rq = (right_bounding_box/(SCREEN_WIDTH/4));
        uint8_t lq = (left_bounding_box/(SCREEN_WIDTH/4));

        if (bq == 4) bq--;
        if (rq == 4) rq--;
        if (lq == 4) lq--;

        // We check the current map.
        bool tl = MAPS[_map_idx-1][tq][lq] == TRANSPARENT_TARGET;
        bool tr = MAPS[_map_idx-1][tq][rq] == TRANSPARENT_TARGET;
        bool bl = MAPS[_map_idx-1][bq][lq] == TRANSPARENT_TARGET;
        bool br = MAPS[_map_idx-1][bq][rq] == TRANSPARENT_TARGET;
        didCollide = !(tl && tr && bl && br);
        if (didCollide) {
            LOG(1, "INFO", "Collision detected !\n");
        }
    }

    prev_action_info.collision = didCollide;

    return didCollide;
}

bool Game::AfterMainMenu() {
    return _common->GetSceneStackIdx() + 1 >= 2;
}

uint8_t Game::GetCenterIdx(uint8_t idx) {
    uint8_t next_idx = (uint8_t)(idx / 4) * 4 + 2;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetCenterIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetNorthIdx(uint8_t idx) {
    // If no next north, return default
    uint8_t next_idx = idx + 3;
    if (next_idx > MAP_SIZE) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetNorthIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthIdx(uint8_t idx) {
    uint8_t next_idx = idx - 3;
    if ((idx - 3) < 1) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetSouthIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetWestIdx(uint8_t idx) {
    uint8_t next_idx = idx - 1;
    if (next_idx < 1) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetEastIdx(uint8_t idx) {
    // For now, consider a row as size 3.
    uint8_t next_idx = idx + 1;
    if (next_idx > MAP_SIZE) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetEastIdx(idx=%i) => %i", idx, next_idx);

    return next_idx;
}

uint8_t Game::GetNorthEastIdx(uint8_t idx) {
    uint8_t next_idx = idx + 4;
    if (idx + 4 > MAP_SIZE) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetNorthEastIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetNorthWestIdx(uint8_t idx) {
    uint8_t next_idx = idx + 2;
    if (idx + 2 > MAP_SIZE) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetNorthWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthEastIdx(uint8_t idx) {
    uint8_t next_idx = idx - 2;
    if (idx - 2 < 1) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetSouthEastIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

uint8_t Game::GetSouthWestIdx(uint8_t idx) {
    uint8_t next_idx = idx - 4;
    if (idx - 4 < 1) next_idx = 0;
    // if (_player_action == PLAYER_ACTION::MOVING) LOG_INFO("GetSouthWestIdx(idx=%i) => %i", idx, next_idx);
    return next_idx;
}

void Game::UpdateMap() {
    if (_player_y < 0) {                    // Move north a map
        map_idx = GetNorthIdx(map_idx);
        _player_y = SCREEN_HEIGHT + _player_y;
        LOG_INFO("Map index moved to: %i\n", map_idx);
    } else if (_player_y >= SCREEN_HEIGHT) { // Move south a map
        map_idx = GetSouthIdx(map_idx);
        _player_y -= SCREEN_HEIGHT;
        LOG_INFO("Map index moved to: %i\n", map_idx);
    } else if (_player_x < 0) {             // Move west a map
        map_idx = GetWestIdx(map_idx);
        LOG_INFO("Map index moved to: %i\n", map_idx);
        _player_x = SCREEN_WIDTH - abs(_player_x);
    } else if (_player_x >= SCREEN_WIDTH) {  // Move east a map
        map_idx = GetEastIdx(map_idx);
        LOG_INFO("Map index moved to: %i\n", map_idx);
        _player_x -= SCREEN_WIDTH;
    }
}

bool IsNotDefaultSprite(const uint8_t map_idx, const uint8_t segment_x, const uint8_t segment_y) {
    return MAPS[map_idx-1][segment_y][segment_x] != -1;
}


SDL_Rect GetSpriteRect(uint8_t map_idx, uint8_t segment_y, uint8_t segment_x) {
    SDL_Rect sprite_rect = {0, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    int v;

    // We can get into a situation where we have no further maps to render.
    // In that case, just render the default which for now is just rows of trees.
    if (map_idx == 0) v = 1;
    // Otherwise if it's a valid map index, get that quadrant's value.
    else v = MAPS[map_idx-1][segment_y][segment_x];

    if (v == -1)  {
        // Consider the default to be the grass texture.
        sprite_rect.x = SPRITE_WIDTH;
        sprite_rect.y = SPRITE_HEIGHT*4;
    } else {
        // Decode the Map encoding.
        uint8_t spritesheet_x = v / 16;
        uint8_t spritesheet_y = v % 16;

        sprite_rect.x = spritesheet_x * SPRITE_WIDTH;
        sprite_rect.y = spritesheet_y * SPRITE_HEIGHT;
    }
    return sprite_rect;
}

void Game::RenderSprite(SDL_Rect& src_rect, SDL_Rect& dst_rect, const uint8_t map_idx, const uint8_t map_segment_y, const uint8_t map_segment_x, SDL_Texture& texture) {
    SDL_Renderer* _renderer = _common->GetRenderer();
    // AdjustBackgroundSprite(&src_rect, map_idx, map_segment_y, map_segment_x);

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

// void whatever() {
//     // Render the north east map
//     if (camera_y < 0 && camera_x > 0) {
//         uint16_t begin_x = 0;
//         uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;
//         uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y))%dst_rect.h);
//         for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = i - camera_x + SCREEN_WIDTH;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
//                 uint16_t delta_map_y = j - begin_y;
//                 dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetNorthEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the north west map
//     if (camera_y < 0 && camera_x < 0) {
//         // We consider either "exactly" the right amount or 16 off.
//         uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.w - abs(camera_x) % dst_rect.w);
//         uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h);

//         for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
//                 uint16_t delta_map_y = j - begin_y;
//                 dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetNorthWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the south east map
//     if (camera_y > 0 && camera_x > 0) {
//         uint16_t begin_x = 0;
//         uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

//         for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = i - camera_x + SCREEN_WIDTH;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
//                 dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetSouthEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the south west map
//     if (camera_y > 0 && camera_x < 0) {
//         // We consider either "exactly" the right amount or 16 off.
//         uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.w - abs(camera_x) % dst_rect.w);

//         for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
//                 dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetSouthWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the south map
//     if (camera_y > 0) {
//         uint16_t begin_x = 0;
//         if (camera_x > dst_rect.w) {
//             begin_x = camera_x - (camera_x % dst_rect.w);
//         }
//         uint16_t end_x = SCREEN_WIDTH;
//         if (camera_x < dst_rect.w) {
//             end_x = SCREEN_WIDTH - camera_x + (camera_x % dst_rect.w);
//         }

//         for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
//             dst_rect.x = i - camera_x;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = 0; j < camera_y+(camera_y % dst_rect.h); j+=dst_rect.h) {
//                 dst_rect.y = (SCREEN_HEIGHT-camera_y) + j;
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetSouthIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the north map
//     if (camera_y < 0) {
//         uint16_t begin_x = 0;
//         if (camera_x > dst_rect.w) {
//             begin_x = camera_x - (camera_x % dst_rect.w);
//         }
//         uint16_t end_x = SCREEN_WIDTH;
//         if (camera_x < dst_rect.w) {
//             end_x = SCREEN_WIDTH - camera_x + (camera_x % dst_rect.w);
//         }

//         // Camera points down.
//         uint16_t begin_y = SCREEN_HEIGHT - abs(camera_y) - ((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h);

//         for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = i - camera_x;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = begin_y; j < SCREEN_HEIGHT; j+=dst_rect.h) {
//                 uint16_t delta_map_y = j - begin_y;
//                 dst_rect.y = delta_map_y - (((SCREEN_HEIGHT - abs(camera_y)) % dst_rect.h) > 0 ? dst_rect.h : 0) + (abs(camera_y) % dst_rect.h);
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetNorthIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the east map
//     if (camera_x > 0) {
//         uint16_t begin_x = 0;
//         uint16_t end_x = SCREEN_WIDTH-camera_x+PLAYER_WIDTH;

//         uint16_t begin_y = 0;
//         if (camera_y > dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);
//         uint16_t end_y = SCREEN_HEIGHT;
//         if (camera_y < - dst_rect.h) end_y = SCREEN_HEIGHT + camera_y + dst_rect.h - (abs(camera_y) % dst_rect.h);

//         for (uint16_t i = begin_x; i < end_x; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = i - camera_x + SCREEN_WIDTH;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
//             for (uint16_t j = begin_y; j < end_y; j+=dst_rect.h) {
//                 uint16_t delta_map_y = j - begin_y;
//                 if (camera_y > 0) {
//                     dst_rect.y = delta_map_y -(abs(camera_y) % dst_rect.h);
//                 } else {
//                     dst_rect.y = delta_map_y + abs(camera_y);
//                 }
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetEastIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }

//     // Render the west map
//     if (camera_x < 0) {
//         // We consider either "exactly" the right amount or 16 off.
//         uint16_t begin_x = SCREEN_WIDTH + camera_x - (dst_rect.h - abs(camera_x) % dst_rect.h);

//         uint32_t begin_y = 0;
//         if (camera_y > dst_rect.h) begin_y = camera_y - (camera_y % dst_rect.h);
//         uint16_t end_y = SCREEN_HEIGHT;
//         if (camera_y < - dst_rect.h) end_y = SCREEN_HEIGHT + camera_y + dst_rect.h - (abs(camera_y) % dst_rect.h);

//         for (uint16_t i = begin_x; i < SCREEN_WIDTH; i+=dst_rect.w) {
//             uint16_t delta_map_x = i - begin_x;
//             dst_rect.x = -dst_rect.w + (abs(camera_x) % dst_rect.w) + delta_map_x;
//             uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));

//             for (uint16_t j = begin_y; j < end_y; j+=dst_rect.h) {
//                 uint16_t delta_map_y = j - begin_y;
//                 if (camera_y > 0) {
//                     dst_rect.y = delta_map_y -(abs(camera_y)%dst_rect.h);
//                 } else {
//                     dst_rect.y = delta_map_y + abs(camera_y);
//                 }
//                 uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
//                 RenderSprite(src_rect, dst_rect, GetWestIdx(map_idx), map_segment_y, map_segment_x, *texture);
//             }
//         }
//     }
// }

// // TOP-MOST MAPS
// if (y < SCREEN_HEIGHT) {
//     if (x >= SCREEN_WIDTH*2) {
//         _map_idx = GetNorthIdx(GetNorthEastIdx(map_idx));
//     } else if (x < SCREEN_WIDTH) {
//         _map_idx = GetNorthIdx(GetNorthWestIdx(map_idx));
//     } else {
//         _map_idx = GetNorthIdx(GetNorthIdx(map_idx));
//     }
// }

// // MIDDLE-MOST MAPS
// if (y >= SCREEN_HEIGHT && y < SCREEN_HEIGHT*2) {
//     if (x > SCREEN_WIDTH*2) {
//         _map_idx = GetNorthIdx(GetEastIdx(map_idx));
//     }
//     else if (x < SCREEN_WIDTH) {
//         _map_idx = GetNorthIdx(GetWestIdx(map_idx));
//     } else {
//         _map_idx = GetNorthIdx(map_idx);
//     }
// }

// BOTTOM-MOST MAPS
// if (y < SCREEN_HEIGHT) {
//     if (x > SCREEN_WIDTH*2) {
//         _map_idx = GetEastIdx(map_idx);
//     }
//     else if (x < SCREEN_WIDTH) {
//         _map_idx = GetWestIdx(map_idx);
//     } else {
//         _map_idx = map_idx;
//     }
// }

void Game::FillBackBufferInitial() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* _back_buffer = _common->GetBackBuffer();
    SDL_SetRenderTarget(_renderer, _back_buffer);
    SDL_RenderClear(_renderer);

    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();

    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF);
    else SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_RenderClear(_renderer);

    // Pre-render the map
    for (uint8_t i = 0; i < current_scene->textures.size(); ++i) {
        uint8_t tag = current_scene->tags[i];
        if (_common->isBackgroundSpriteTexture(tag)) {
            SDL_Texture* texture = current_scene->textures[i]; // Spritesheet texture
            SDL_Rect dst = { 0, 0, BG_SPRITE_WIDTH, BG_SPRITE_HEIGHT };
            for (int x = 0; x < BACKBUFFER_WIDTH; x+=BG_SPRITE_WIDTH) {
                const uint16_t map_x = x % SCREEN_WIDTH;
                uint8_t map_segment_x = (uint8_t)(map_x/(SCREEN_WIDTH/4));
                dst.x = x;
                uint8_t _map_idx = map_idx;
                for (int y = 0; y < BACKBUFFER_HEIGHT; y+=dst.h) {
                    const uint16_t map_y = y % SCREEN_HEIGHT;
                    uint8_t map_segment_y = (uint8_t)(map_y/(SCREEN_HEIGHT/4));
                    dst.y = y;

                    // SDL_Rect GetSpriteRect(uint8_t map_idx, uint8_t segment_y, uint8_t segment_x);
                    SDL_Rect src = GetSpriteRect(_map_idx, map_segment_y, map_segment_x);

                    // LOG_INFO("<Source rectangle x=%i, y=%i, w=%i, h=%i>\n", src.x, src.y, src.w, src.h);
                    // LOG_INFO("<Dest rectangle x=%i, y=%i, w=%i, h=%i>\n", dst.x, dst.y, dst.w, dst.h);

                    int v = MAPS[_map_idx-1][map_segment_y][map_segment_x];
                    if (v == -1) {
                        SDL_RenderCopy(_renderer, texture, &src, &dst);
                    } else {
                        // Render the grass first
                        SDL_RenderCopy(_renderer, texture, &grass_rect, &dst);

                        // Render the actual sprite
                        SDL_RenderCopy(_renderer, texture, &src, &dst);
                    }
                    // return;  // Temporary.
                }
            }
        }
   }
}

void Game::UpdateCamera() {
    _camera_x = _player_x - PLAYER_BEGIN_X; // X grows negatively when moving left in a given map.
    _camera_y = _player_y - PLAYER_BEGIN_Y; // Y grows positively when moving down in a given map.
    LOG_nodt(1, "INFO", "<Camera x=%i, y=%i>\n", _camera_x, _camera_y);
    LOG_nodt(1, "INFO", "<Scroll x=%i, y=%i>\n", _scroll_x, _scroll_y);
}

void Game::BlitNext() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* _back_buffer = _common->GetBackBuffer();

    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();

    SDL_Texture* t = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        BACKBUFFER_WIDTH,
        BACKBUFFER_HEIGHT
    );

    // Copy the shifted back buffer into our new texture.
    SDL_SetRenderTarget(_renderer, t);

    SDL_Rect src;
    SDL_Rect dst;
    SDL_Rect _grass_strip;

    switch(_player_direction) {
    case PLAYER_DIRECTION::UP:
        src = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        dst = {0, STEP_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        _grass_strip = {SPRITE_WIDTH, SPRITE_HEIGHT*4, BG_SPRITE_WIDTH, STEP_SIZE};
        break;
    case PLAYER_DIRECTION::DOWN:
        src = {0, STEP_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        _grass_strip = {SPRITE_WIDTH, SPRITE_HEIGHT*4, BG_SPRITE_WIDTH, STEP_SIZE};
        break;
    case PLAYER_DIRECTION::LEFT:
        src = {0, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        dst = {STEP_SIZE, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        _grass_strip = {SPRITE_WIDTH, SPRITE_HEIGHT*4, STEP_SIZE, BG_SPRITE_HEIGHT};
        break;
    case PLAYER_DIRECTION::RIGHT:
        src = {STEP_SIZE, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        dst = {0, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        _grass_strip = {SPRITE_WIDTH, SPRITE_HEIGHT*4, STEP_SIZE, BG_SPRITE_HEIGHT};
        break;
    }

    LOG_nodt(1, "INFO", "Move <FrontBuffer x=%i, y=%i, w=%i, h=%i> to <BackBuffer x=%i, y=%i, w=%i, h=%i>\n", src.x, src.y, src.w, src.h, dst.x, dst.y, dst.w, dst.h);

    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);

    int map_y, map_x = 0;

    if (_camera_y < 0) {        // Camera shows the map above.
        map_y = SCREEN_HEIGHT - abs(_camera_y);
    } else if (_camera_y > 0) { // Camera shows the map below.
        map_y = _camera_y;
    }

    if (_camera_x < 0) {        // Camera shows the map to the left.
        map_x = SCREEN_WIDTH - abs(_camera_x);
    } else if (_camera_x > 0) { // Camera shows the map to the right.
        map_x = _camera_x;
    }

    uint8_t map_idx_buffer[2] = {0, 0};
    int buf_idx = 0;

    if (!(_camera_y == 0 && _camera_x == 0)) {
        switch(_player_direction) {
        case PLAYER_DIRECTION::UP:
            if (_camera_y < 0) {
                if (_camera_x < 0) {
                    map_idx_buffer[buf_idx++] = GetNorthWestIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                if (_camera_x > 0) {
                    map_idx_buffer[buf_idx++] = GetNorthEastIdx(map_idx);
                }
            } else {
                if (_camera_x < 0) {
                    map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = map_idx; // Still the map the player is in.
                if (_camera_x > 0) {
                    map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
                }
            }
            break;
        case PLAYER_DIRECTION::DOWN:
            if (_camera_y > 0) {
                if (_camera_x < 0) {
                    map_idx_buffer[buf_idx++] = GetSouthWestIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
                if (_camera_x > 0) {
                    map_idx_buffer[buf_idx++] = GetSouthEastIdx(map_idx);
                }
            } else {
                if (_camera_x < 0) {
                    map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = map_idx; // Still the map the player is in.
                if (_camera_x > 0) {
                    map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
                }
            }
            break;
        case PLAYER_DIRECTION::RIGHT:
            if (_camera_x < 0) {
                if (_camera_y < 0) {
                    map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = map_idx; // Still the map the player is in.
                if (_camera_y > 0) {
                    map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
                }
            } else {
                if (_camera_y < 0) {
                    map_idx_buffer[buf_idx++] = GetNorthEastIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
                if (_camera_y > 0) {
                    map_idx_buffer[buf_idx++] = GetSouthEastIdx(map_idx);
                }
            }
            break;
        case PLAYER_DIRECTION::LEFT:
            if (_camera_x > 0) {
                if (_camera_y < 0) {
                    map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = map_idx; // Still the map the player is in.
                if (_camera_y > 0) {
                    map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
                }
            } else {
                if (_camera_y < 0) {
                    map_idx_buffer[buf_idx++] = GetNorthWestIdx(map_idx);
                }
                map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                if (_camera_y > 0) {
                    map_idx_buffer[buf_idx++] = GetSouthWestIdx(map_idx);
                }
            }
            break;
        }
    } else map_idx_buffer[buf_idx++] = map_idx;

    buf_idx = 0;

    // Where the strip will be.
    int blit_placement_y = 0;
    int blit_placement_x = 0;

    if (_player_direction      == PLAYER_DIRECTION::DOWN)  blit_placement_y = SCREEN_HEIGHT - STEP_SIZE;
    else if (_player_direction == PLAYER_DIRECTION::UP)    blit_placement_y = 0;
    else if (_player_direction == PLAYER_DIRECTION::RIGHT) blit_placement_x = SCREEN_WIDTH - STEP_SIZE;
    else if (_player_direction == PLAYER_DIRECTION::LEFT)  blit_placement_x = 0;

    // LOG_nodt(1, "INFO", "<ViewportStrip y=%i, x=%i, w=%i, h=%i>\n", blit_placement_y, blit_placement_x, _grass_strip.w, _grass_strip.h);
    LOG_nodt(1, "INFO", "<MapStrip offset_y=%i, offset_x=%i, map_idxs=[%zu, %zu]>\n", map_y, map_x, map_idx_buffer[0], map_idx_buffer[1]);
    LOG_nodt(1, "INFO", "<Player y=%i, x=%i, map_idx=%zu>\n", _player_y, _player_x, map_idx);

    SDL_Texture* spritesheet;

    for (uint8_t i = 0; i < current_scene->textures.size(); ++i) {
        uint8_t tag = current_scene->tags[i];
        if (_common->isBackgroundSpriteTexture(tag)) {
            spritesheet = current_scene->textures[i];
            break;
        }
    }

    if (_player_direction == PLAYER_DIRECTION::LEFT) {
        if (_tile_offset_x == 0) _tile_offset_x = 40;
        else                     _tile_offset_x -= STEP_SIZE;
    }
    if (_player_direction == PLAYER_DIRECTION::RIGHT) {
        if (_tile_offset_x == 40) _tile_offset_x = 0;
        else                     _tile_offset_x += STEP_SIZE;
    }
    if (_player_direction == PLAYER_DIRECTION::UP) {
        if (_tile_offset_y == 0) _tile_offset_y = 40;
        else                      _tile_offset_y -= STEP_SIZE;
    }
    if (_player_direction == PLAYER_DIRECTION::DOWN) {
        if (_tile_offset_y == 40) _tile_offset_y = 0;
        else                      _tile_offset_y += STEP_SIZE;
    }

    int tile_offset_y = _tile_offset_y;
    int tile_offset_x = _tile_offset_x;

    LOG_nodt(1, "INFO", "Tile offset y - %i\n", tile_offset_y);
    LOG_nodt(1, "INFO", "Tile offset x - %i\n", tile_offset_x);

    assert(tile_offset_x % STEP_SIZE == 0);
    assert(tile_offset_x >= 0);
    assert(tile_offset_y % STEP_SIZE == 0);
    assert(tile_offset_y >= 0);
    assert(_grass_strip.w > 0);
    assert(_grass_strip.h > 0);

    int end_x = (_player_direction == PLAYER_DIRECTION::UP   || _player_direction == PLAYER_DIRECTION::DOWN) ? SCREEN_WIDTH : STEP_SIZE;
    int end_y = (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) ? SCREEN_HEIGHT : STEP_SIZE;

    uint8_t map_segment_y, map_segment_x;

    _grass_strip.y += tile_offset_y;
    _grass_strip.x += tile_offset_x;

    if (_player_direction == PLAYER_DIRECTION::UP || _player_direction == PLAYER_DIRECTION::DOWN) {
        for (int x = 0; x < end_x; x+=BG_SPRITE_WIDTH) {
            // Do a quick check to see if we need to alter _map_idx.
            if (map_x + x >= SCREEN_WIDTH && buf_idx == 0) buf_idx++;

            int __map_x = (map_x+x) - (SCREEN_WIDTH * buf_idx);
            map_segment_x = (uint8_t)(__map_x/(SCREEN_WIDTH/4)) % 4;
            map_segment_y = (uint8_t)(map_y/(SCREEN_HEIGHT/4)) % 4;

            SDL_Rect dst_strip = {x, blit_placement_y, _grass_strip.w, _grass_strip.h};
            SDL_Rect sprite_rect = GetSpriteRect(map_idx_buffer[buf_idx], map_segment_y, map_segment_x);
            SDL_Rect src_strip = {sprite_rect.x + tile_offset_x, sprite_rect.y + tile_offset_y, _grass_strip.w, _grass_strip.h};

            int sprite_y = sprite_rect.y + tile_offset_y;
            int sprite_x = sprite_rect.x + tile_offset_x;

            assert(sprite_x % BG_SPRITE_WIDTH == _grass_strip.x % BG_SPRITE_WIDTH);
            assert(sprite_y % BG_SPRITE_HEIGHT == _grass_strip.y % BG_SPRITE_HEIGHT);
            assert(sprite_x >= 0);
            assert(sprite_y >= 0);

            _grass_strip.x -= tile_offset_x;
            x -= tile_offset_x;
            tile_offset_x = 0; // We only action the first tile offset.

            int v = MAPS[map_idx_buffer[buf_idx]-1][map_segment_y][map_segment_x];
            if (v == -1) SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);
            else {
                SDL_RenderCopy(_renderer, spritesheet, &_grass_strip, &dst_strip); // Render the grass first
                SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);    // Overlay the actual sprite
            }
        }
    } else if (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) {
        for (int y = 0; y < end_y; y+=BG_SPRITE_HEIGHT) {     // all the sprites in Y-Axis for map.
            if (map_y + y >= SCREEN_HEIGHT && buf_idx == 0)  buf_idx++;

            // keep the iterated y state in-tact but we might've moved into grabbing pixels from the next map.
            int __map_y = (map_y+y) - (SCREEN_HEIGHT * buf_idx);
            map_segment_x = (uint8_t)(map_x/(SCREEN_WIDTH/4)) % 4;
            map_segment_y = (uint8_t)(__map_y/(SCREEN_HEIGHT/4)) % 4;

            SDL_Rect dst_strip = {blit_placement_x, y, _grass_strip.w, _grass_strip.h};
            SDL_Rect sprite_rect = GetSpriteRect(map_idx_buffer[buf_idx], map_segment_y, map_segment_x);
            SDL_Rect src_strip = {sprite_rect.x + tile_offset_x, sprite_rect.y + tile_offset_y, _grass_strip.w, _grass_strip.h};

            int sprite_y = sprite_rect.y + tile_offset_y;
            int sprite_x = sprite_rect.x + tile_offset_x;

            assert(sprite_x % BG_SPRITE_WIDTH == _grass_strip.x % BG_SPRITE_WIDTH);
            assert(sprite_y % BG_SPRITE_HEIGHT == _grass_strip.y % BG_SPRITE_HEIGHT);
            assert(sprite_x >= 0);
            assert(sprite_y >= 0);

            _grass_strip.y -= tile_offset_y;
            y -= tile_offset_y;
            tile_offset_y = 0; // We only action the first tile offset.

            int v = MAPS[map_idx_buffer[buf_idx]-1][map_segment_y][map_segment_x];
            if (v == -1) SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);
            else {
                SDL_RenderCopy(_renderer, spritesheet, &_grass_strip, &dst_strip); // Render the grass first
                SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);    // Overlay the actual sprite
            }
    }
}


    SDL_DestroyTexture(_back_buffer);
    _common->SetBackBuffer(t);
}

void Game::UpdateBackBuffer() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* _back_buffer = _common->GetBackBuffer();
    SDL_SetRenderTarget(_renderer, NULL);                // Set the renderer to target the screen buffer now.
    SDL_RenderClear(_renderer);                          // Un-apply the front-buffer.
    SDL_RenderCopy(_renderer, _back_buffer, NULL, NULL); // Apply the back buffer

    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();

    // FIXME: For now we just render the player sprite as-is
    //        but in theory this can be it's own separate buffer.
    SDL_Rect src_player_rect;
    SDL_Rect dst_player_rect;
    SDL_Texture* player_texture;
    uint8_t i = 0;
    for (; i < current_scene->textures.size(); ++i) {
        if (_common->isPlayerSpriteTexture(current_scene->tags[i])) {
            player_texture = current_scene->textures[i];
            src_player_rect = current_scene->texture_src_rects[i];
            dst_player_rect = current_scene->texture_dst_rects[i];
            break;
        }
    }

    int __scene_stack_idx = (int)_scene_stack_idx;

    if (__scene_stack_idx > 0) {
        if (_attack_animation.active && _attack_animation.runtime > 0) {
            LOG_INFO("Attack animation x: %i\n", _attack_animation.x);
            _attack_animation.runtime -= 1;
            src_player_rect.y = PLAYER_HEIGHT;
            if (_attack_animation.runtime >= ATTACK_ANIMATION_FRAMES/2) src_player_rect.x = PLAYER_WIDTH;
            else                                                        src_player_rect.x = PLAYER_WIDTH*2;

            float dt = ATTACK_ANIMATION_FRAMES+1 - _attack_animation.runtime / ATTACK_ANIMATION_FRAMES;
            _attack_animation.x += 0.125 * dt;
            dst_player_rect.x = _attack_animation.x;
        } else {
            dst_player_rect.x = PLAYER_BEGIN_X;
            dst_player_rect.y = PLAYER_BEGIN_Y;
            if (_attack_animation.active) ResetAttackAnimation();
            if (_player_action == PLAYER_ACTION::MOVING) {
                if (_move_animation.remaining_ticks == 0 || _move_animation.direction != _player_direction) {
                    _move_animation.remaining_ticks = MOVE_ANIM_TICKS;
                    _move_animation.direction = _player_direction;
                } else {
                    TickPlayerMove();
                }
                if (_move_animation.direction == PLAYER_DIRECTION::UP) {
                    if (_move_animation.remaining_ticks <= MOVE_ANIM_TICKS/2) src_player_rect.x = PLAYER_WIDTH*3;
                    else                                                      src_player_rect.x = PLAYER_WIDTH*4;
                } else {
                    if (_move_animation.remaining_ticks <= MOVE_ANIM_TICKS/2) src_player_rect.x = 0;
                    else                                                      src_player_rect.x = PLAYER_WIDTH * 2;
                }
                src_player_rect.y = 0;
            } else if (_player_action == PLAYER_ACTION::STOPPED) {
                src_player_rect.x = PLAYER_WIDTH;
                src_player_rect.y = 0;
            }
        }

        dst_player_rect.x = PLAYER_BEGIN_X;
        dst_player_rect.y = PLAYER_BEGIN_Y;
        current_scene->texture_src_rects[i] = src_player_rect;
        SDL_RenderCopy(_renderer, player_texture, &src_player_rect, &dst_player_rect);

        DrawPlayerBoundingBox(); // Temporarily draw the bounding box.
    }

    for (; i < current_scene->textures.size(); ++i) {
        SDL_Texture* texture = current_scene->textures[i];
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        if (_common->isTextTexture(tag)) {
            // FIXME: This is a hack, we want to first draw the quad and then the text.
            // Menu quad. (optional)
            if (_game_state == game_state_t::PAUSE) {
                float pause_x = SCREEN_WIDTH * 0.35;
                float pause_y = SCREEN_HEIGHT * 0.25;
                float pause_w = SCREEN_WIDTH * 0.3;
                float pause_h = SCREEN_HEIGHT * 0.4;
                SDL_Rect fillRect = { pause_x, pause_y, pause_w, pause_h };
                SDL_SetRenderDrawColor( _renderer, 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderFillRect( _renderer, &fillRect );
            }
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        }
    }

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
        const uint16_t dx = i - _camera_x;
        uint8_t map_segment_x = (uint8_t)(i/(SCREEN_WIDTH/4));
        for (uint16_t j = 0; j < SCREEN_HEIGHT; j+=SCREEN_HEIGHT/4) {
            uint8_t map_segment_y = (uint8_t)(j/(SCREEN_HEIGHT/4));
            const uint16_t dy = j - _camera_y;
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

    // if (_scene_stack_idx == 0) return;
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
           .font_size = FONT_SIZE::LARGE,
           .tag = TEXT_TAG
       };

       _common->LoadTexture(_scene_stack_idx, fps_texture);

       if (_game_state == game_state_t::PAUSE) {
           //Render main menu (optionally)
           int save_x = SCREEN_WIDTH*0.375 + (SCREEN_WIDTH*0.1);
           int save_y = SCREEN_HEIGHT*0.25 + (SCREEN_HEIGHT*0.1);
           gametexture_t menu_text_texture = {
               .text_or_uri = "SAVE",
               .src_rect = {0, 0, 0, 0},
               .dst_rect = {save_x, save_y, 0, 0},
               .color = {255,255,255,255},
               .font_size = FONT_SIZE::MEDIUM,
               .tag = TEXT_TAG
           };
           _common->LoadTexture(_scene_stack_idx, menu_text_texture);
       }
    }
    SDL_RenderPresent(_renderer);                        // Present the front buffer.
}

void Game::HandleSpaceKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (!AfterMainMenu()) {
        _common->AllocateScene(true);
        FillBackBufferInitial();
    }
    else {
        SetPlayerAction(PLAYER_ACTION::ATTACK);
        SetAttackAnimation(ATTACK_ANIMATION_FRAMES, true);
    }
}

void Game::HandleUpKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (prev_action_info.key == SDLK_UP && prev_action_info.collision) return;
    if (IsColliding(_player_x, _player_y - STEP_SIZE)) return;
    _player_action = PLAYER_ACTION::MOVING;
    _player_direction = PLAYER_DIRECTION::UP;
    _scroll_y -= STEP_SIZE;
    _player_y -= STEP_SIZE;
    UpdateMap();
    UpdateCamera();
    BlitNext();
}

void Game::HandleDownKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (prev_action_info.key == SDLK_DOWN && prev_action_info.collision) return;
    if (IsColliding(_player_x, _player_y + STEP_SIZE)) return;
    _player_action = PLAYER_ACTION::MOVING;
    _player_direction = PLAYER_DIRECTION::DOWN;
    _scroll_y += STEP_SIZE;
    _player_y += STEP_SIZE;
    UpdateMap();
    UpdateCamera();
    BlitNext();
}

void Game::HandleLeftKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (prev_action_info.key == SDLK_LEFT && prev_action_info.collision) return;
    if (IsColliding(_player_x - STEP_SIZE, _player_y)) return;
    _player_action = PLAYER_ACTION::MOVING;
    _player_direction = PLAYER_DIRECTION::LEFT;
    _scroll_x -= STEP_SIZE;
    _player_x -= STEP_SIZE;
    UpdateMap();
    UpdateCamera();
    BlitNext();
}

void Game::HandleRightKey() {
    if (_game_state == game_state_t::PAUSE) return;
    if (AttackAnimationActive()) return;
    if (prev_action_info.key == SDLK_RIGHT && prev_action_info.collision) return;
    if (IsColliding(_player_x + STEP_SIZE, _player_y)) return;
    _player_action = PLAYER_ACTION::MOVING;
    _player_direction = PLAYER_DIRECTION::RIGHT;
    _scroll_x += STEP_SIZE;
    _player_x += STEP_SIZE;
    UpdateCamera();
    UpdateMap();
    BlitNext();
}

void Game::HandleEscKey() {
    LOG_INFO("Escape key pressed\n");
    if (_game_state == game_state_t::PAUSE) _game_state = game_state_t::PLAY;
    else _game_state = game_state_t::PAUSE;
}

int main() {
    std::string app_name = std::string("Game");
    std::shared_ptr<Common> common_ptr = std::make_shared<Common>(std::move(app_name), BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
    std::unique_ptr<Game> game = std::make_unique<Game>(common_ptr);

    int quit_app = 0;
    Uint64 tick = SDL_GetTicks64();     // SDL Library is initialized above via Common::Common()
    Uint64 last_frame_tick = tick - 17; // Ensures we always render the first frame straight-away.
    Uint64 dt_frame = 17;               // Ensures no initial event wait.
    Uint64 frame_per_second = 0;        // Counter that gets reset after 1s.
    Uint64 seconds = 1;                 // Seconds rendered.
    SDL_Event eh;
    Uint64 before = SDL_GetTicks64();
    game->FillBackBufferInitial();
    LOG_INFO("Fill back buffer initial took: %zums\n", SDL_GetTicks64()-before);

    int DEFAULT_WAIT = 13;
    int timeout = 0;

    while (!quit_app) {
        if (dt_frame < DEFAULT_WAIT && timeout != 0) timeout = DEFAULT_WAIT - dt_frame; // Allow 3ms to draw or handle interaction.
        // LOG_INFO("Block for: %ims\n", timeout);

        while (SDL_WaitEventTimeout(&eh, timeout) != 0) {
            if (eh.type == SDL_QUIT) { quit_app = 1; break; }
            if (eh.type == SDL_KEYDOWN) {
                SDL_KeyboardEvent key = eh.key;
                SDL_Keysym keysym = key.keysym;

                switch(keysym.sym) {
                case SDLK_SPACE:
                    game->HandleSpaceKey();
                    break;
                case SDLK_UP:
                    game->HandleUpKey();
                    game->SetPrevActionInfoKey(SDLK_UP);
                    break;
                case SDLK_DOWN:
                    game->HandleDownKey();
                    game->SetPrevActionInfoKey(SDLK_DOWN);
                    break;
                case SDLK_LEFT:
                    game->HandleLeftKey();
                    game->SetPrevActionInfoKey(SDLK_LEFT);
                    break;
                case SDLK_RIGHT:
                    game->HandleRightKey();
                    game->SetPrevActionInfoKey(SDLK_RIGHT);
                    break;
                case SDLK_ESCAPE:
                    game->HandleEscKey();
                    break;
                }
            }
            if (eh.type == SDL_KEYUP) { game->SetPlayerAction(PLAYER_ACTION::STOPPED); }
        }
        tick = SDL_GetTicks64();
        dt_frame = tick - last_frame_tick;
        // LOG_INFO("tick: %zu, dt_frame: %zu\n", tick, dt_frame);
        if (dt_frame > DEFAULT_WAIT) { // Allow 2ms draw time.
            Uint64 before = SDL_GetTicks64();
            game->UpdateBackBuffer();
            Uint64 after = SDL_GetTicks64();
            // LOG_INFO("[DRAW]\n");
            // LOG_INFO("Update back buffer took: %zums\n", SDL_GetTicks64()-before);
            frame_per_second++;
            if (tick > 1000*seconds) {
                game->set_fps(frame_per_second);
                seconds++;
                frame_per_second=0;
                timeout = 13;
            }
            frame_count++;
            last_frame_tick = tick;
            // LOG_INFO("last_frame_tick: %zu\n", last_frame_tick);
            dt_frame = SDL_GetTicks64() - after;
        }
    }
    return 0;
}

// TODO: Going south has an off by STEP_SIZE issue.
// TODO: Mixing up/down with left/right leads to incorrect offsets somehow.




// TODO: Draw minimap.
// TODO: Menu on ESC.
// TODO: Save game.
// TODO: First use player sprite as NPC to test out dialog.
