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
    int _screen_center = SCREEN_CENTER;
    const vector<gametexture_t> SCENE_1 = {
        { .text_or_uri = "Forged Memories",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {_screen_center, 50, 0, 0},
          .color = {255,255,255,255},
          .font_size = FONT_SIZE::MEDIUM,
          .tag = TEXT_TAG
        },
        { .text_or_uri = "<SPC> to play",
          .src_rect = {0, 0, 0, 0},
          .dst_rect = {_screen_center, SCREEN_HEIGHT/2 - 1, 0, 0},
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

uint8_t Game::GetNorthIdx(uint8_t idx) {
    uint8_t next_idx = idx + 3;
    if (next_idx > MAP_SIZE) next_idx = 0;
    return next_idx;
}

uint8_t Game::GetSouthIdx(uint8_t idx) {
    if (idx < 3) return 0;
    return idx - 3;
}

uint8_t Game::GetWestIdx(uint8_t idx) {
    if (idx == 0) return 0;
    return idx - 1;
}

uint8_t Game::GetEastIdx(uint8_t idx) {
    uint8_t next_idx = idx + 1;
    if (next_idx > MAP_SIZE) next_idx = 0;
    return next_idx;
}

uint8_t Game::GetNorthEastIdx(uint8_t idx) {
    uint8_t next_idx = idx + 4;
    if (idx + 4 > MAP_SIZE) next_idx = 0;
    return next_idx;
}

uint8_t Game::GetNorthWestIdx(uint8_t idx) {
    uint8_t next_idx = idx + 2;
    if (idx + 2 > MAP_SIZE) next_idx = 0;
    return next_idx;
}

uint8_t Game::GetSouthEastIdx(uint8_t idx) {
    if (idx < 2) return 0;
    return idx - 2;
}

uint8_t Game::GetSouthWestIdx(uint8_t idx) {
    if (idx < 4) return 0;
    return idx - 4;
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

   SDL_RenderDrawLine( _renderer, begin_x, begin_y, end_x, begin_y);  // Top line
   SDL_RenderDrawLine( _renderer, begin_x, begin_y, begin_x, end_y);  // Left line
   SDL_RenderDrawLine( _renderer, end_x, begin_y, end_x, end_y);      // Right line
   SDL_RenderDrawLine( _renderer, begin_x, end_y, end_x, end_y);      // Bottom line
}

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

void Game::GetNextBackBuffer(SDL_Texture* t) {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* _back_buffer = _common->GetBackBuffer();

    SDL_Rect src;
    SDL_Rect dst;

    switch(_player_direction) {
    case PLAYER_DIRECTION::UP:
        src = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        dst = {0, STEP_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        break;
    case PLAYER_DIRECTION::DOWN:
        src = {0, STEP_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-STEP_SIZE};
        break;
    case PLAYER_DIRECTION::LEFT:
        src = {0, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        dst = {STEP_SIZE, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        break;
    case PLAYER_DIRECTION::RIGHT:
        src = {STEP_SIZE, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        dst = {0, 0, SCREEN_WIDTH-STEP_SIZE, SCREEN_HEIGHT};
        break;
    }

    SDL_SetRenderTarget(_renderer, t);                              // Switch to our temporary back buffer holder texture.
    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);
}

void Game::BlitNext() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* t = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        BACKBUFFER_WIDTH,
        BACKBUFFER_HEIGHT
    );
    GetNextBackBuffer(t);

    SDL_Texture* _back_buffer = _common->GetBackBuffer();

    int top_strip_y = 0, bottom_strip_y, left_strip_x, right_strip_x = 0;
    int map_y, map_x = 0;

    if (_camera_y < 0) {
        top_strip_y = SCREEN_HEIGHT - abs(_camera_y);
    } else {
        top_strip_y = _camera_y;
    }
    if (_camera_y < 0) {
        bottom_strip_y = top_strip_y - STEP_SIZE;
    } else {
        bottom_strip_y = ((top_strip_y + SCREEN_HEIGHT) - STEP_SIZE) % SCREEN_HEIGHT;
    }

    if (_player_direction == PLAYER_DIRECTION::UP) {
        map_y = top_strip_y;
    }
    if (_player_direction == PLAYER_DIRECTION::DOWN) {
        map_y = bottom_strip_y;
    }
    if (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) {
        map_y = top_strip_y;
    }

    if (_camera_x < 0) {
        left_strip_x = SCREEN_WIDTH - abs(_camera_x);
    } else {
        left_strip_x = _camera_x;
    }
    if (_camera_x < 0) {
        right_strip_x = left_strip_x - STEP_SIZE;
    } else {
        right_strip_x = ((left_strip_x + SCREEN_WIDTH) - STEP_SIZE) % SCREEN_WIDTH;
    }
    if (_player_direction == PLAYER_DIRECTION::LEFT) {
        map_x = left_strip_x;
    }
    if (_player_direction == PLAYER_DIRECTION::RIGHT) {
        map_x = right_strip_x;
    }
    if (_player_direction == PLAYER_DIRECTION::UP || _player_direction == PLAYER_DIRECTION::DOWN) {
        map_x = left_strip_x;
    }

    LOG_nodt(1, "INFO", "top_strip_y=%i\n", top_strip_y);
    LOG_nodt(1, "INFO", "bottom_strip_y=%i\n", bottom_strip_y);
    LOG_nodt(1, "INFO", "left_strip_x=%i\n", left_strip_x);
    LOG_nodt(1, "INFO", "right_strip_x=%i\n", right_strip_x);

    LOG_nodt(1, "INFO", "map_y=%i\n", map_y);
    LOG_nodt(1, "INFO", "map_x=%i\n", map_x);

    if (_player_direction == PLAYER_DIRECTION::UP)    {
        _tile_offset_y = top_strip_y % BG_SPRITE_HEIGHT;
    }
    if (_player_direction == PLAYER_DIRECTION::DOWN)  {
        _tile_offset_y = bottom_strip_y % BG_SPRITE_HEIGHT;
    }
    if (_player_direction == PLAYER_DIRECTION::LEFT)  {
        _tile_offset_x = left_strip_x % BG_SPRITE_WIDTH;
    }
    if (_player_direction == PLAYER_DIRECTION::RIGHT) {
        _tile_offset_x = right_strip_x % BG_SPRITE_WIDTH;
    }

    if (_player_direction == PLAYER_DIRECTION::UP || _player_direction == PLAYER_DIRECTION::DOWN) {
        _tile_offset_x = left_strip_x % BG_SPRITE_WIDTH;
    }
    if (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) {
        _tile_offset_y = top_strip_y % BG_SPRITE_HEIGHT;
    }

    // LOG_nodt(1, "INFO", "_tile_offset_y=%i\n", _tile_offset_y);
    // LOG_nodt(1, "INFO", "_tile_offset_x=%i\n", _tile_offset_x);
    assert(_tile_offset_y >= 0 && _tile_offset_y < 50);
    assert(_tile_offset_x >= 0 && _tile_offset_x < 50);

    uint8_t map_idx_buffer[2] = {0, 0};
    int buf_idx = 0;

    // Accumulate maps from which the strip could reside in.
    if (!(_camera_y == 0 && _camera_x == 0)) {
        switch(_player_direction) {
        case PLAYER_DIRECTION::UP:
            if (_camera_y < 0) {
                if (_camera_x < 0) map_idx_buffer[buf_idx++] = GetNorthWestIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                if (_camera_x > 0) map_idx_buffer[buf_idx++] = GetNorthEastIdx(map_idx);
            } else {
                if (_camera_x < 0) map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = map_idx;
                if (_camera_x > 0) map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
            }
            break;
        case PLAYER_DIRECTION::DOWN:
            if (_camera_y > 0) {
                if (_camera_x < 0) map_idx_buffer[buf_idx++] = GetSouthWestIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
                if (_camera_x > 0) map_idx_buffer[buf_idx++] = GetSouthEastIdx(map_idx);
            } else {
                if (_camera_x < 0) map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = map_idx;
                if (_camera_x > 0) map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
            }
            break;
        case PLAYER_DIRECTION::RIGHT:
            if (_camera_x > 0) {
                if (_camera_y < 0) map_idx_buffer[buf_idx++] = GetNorthEastIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = GetEastIdx(map_idx);
                if (_camera_y > 0) map_idx_buffer[buf_idx++] = GetSouthEastIdx(map_idx);
            }
            if (_camera_x <= 0) {
                if (_camera_y < 0) map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = map_idx;
                if (_camera_y > 0) map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
            }
            break;
        case PLAYER_DIRECTION::LEFT:
            if (_camera_x >= 0) {
                if (_camera_y < 0) map_idx_buffer[buf_idx++] = GetNorthIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = map_idx;
                if (_camera_y > 0) map_idx_buffer[buf_idx++] = GetSouthIdx(map_idx);
            }
            if (_camera_x < 0) {
                if (_camera_y < 0) map_idx_buffer[buf_idx++] = GetNorthWestIdx(map_idx);
                                   map_idx_buffer[buf_idx++] = GetWestIdx(map_idx);
                if (_camera_y > 0) map_idx_buffer[buf_idx++] = GetSouthWestIdx(map_idx);
            }
            break;
        }
    } else map_idx_buffer[buf_idx++] = map_idx;

    buf_idx = 0;

    // Where the strip will be.
    int blit_placement_y = 0;
    int blit_placement_x = 0;

    if (_player_direction == PLAYER_DIRECTION::UP)              blit_placement_y = 0;
    else if (_player_direction      == PLAYER_DIRECTION::DOWN)  blit_placement_y = SCREEN_HEIGHT - STEP_SIZE;
    else if (_player_direction == PLAYER_DIRECTION::LEFT)       blit_placement_x = 0;
    else if (_player_direction == PLAYER_DIRECTION::RIGHT)      blit_placement_x = SCREEN_WIDTH - STEP_SIZE;

    LOG_nodt(1, "INFO", "<MapStrip offset_y=%i, offset_x=%i, map_idxs=[%zu, %zu]>\n", map_y, map_x, map_idx_buffer[0], map_idx_buffer[1]);
    LOG_nodt(1, "INFO", "<Player y=%i, x=%i, map_idx=%zu>\n", _player_y, _player_x, map_idx);

    scene_t* current_scene = _common->GetCurrentScene();

    // TODO: We should just hold onto the spritesheet texture as a private field.
    SDL_Texture* spritesheet;
    for (uint8_t i = 0; i < current_scene->textures.size(); ++i) {
        uint8_t tag = current_scene->tags[i];
        if (_common->isBackgroundSpriteTexture(tag)) { spritesheet = current_scene->textures[i]; break; }
    }

    uint8_t tile_offset_y = _tile_offset_y;
    uint8_t tile_offset_x = _tile_offset_x;

    LOG_nodt(1, "INFO", "Tile offset y - %i\n", tile_offset_y);
    LOG_nodt(1, "INFO", "Tile offset x - %i\n", tile_offset_x);

    assert(tile_offset_x % STEP_SIZE == 0);
    assert(tile_offset_y % STEP_SIZE == 0);
    assert(tile_offset_x >= 0);
    assert(tile_offset_y >= 0);

    int end_x = (_player_direction == PLAYER_DIRECTION::UP   || _player_direction == PLAYER_DIRECTION::DOWN)  ? SCREEN_WIDTH  : STEP_SIZE;
    int end_y = (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) ? SCREEN_HEIGHT : STEP_SIZE;

    uint8_t map_segment_y, map_segment_x;

    if (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) {
        ASSERT_AND_LOG((map_x - tile_offset_x) % BG_SPRITE_WIDTH == 0, "map_x=%i, tile_offset_x=%i", map_x, tile_offset_x);
    }
    if (_player_direction == PLAYER_DIRECTION::UP || _player_direction == PLAYER_DIRECTION::DOWN) {
        ASSERT_AND_LOG((map_y - tile_offset_y) % BG_SPRITE_HEIGHT == 0, "map_y=%i, tile_offset_y=%i", map_y, tile_offset_y);
    }

    if (_player_direction == PLAYER_DIRECTION::UP || _player_direction == PLAYER_DIRECTION::DOWN) {
        for (int x = 0; x < end_x; x+=BG_SPRITE_WIDTH) {
            if (map_x + x == SCREEN_WIDTH) { buf_idx++; }              // If during vertical strip, we move to a next map
            if (buf_idx == 0) ASSERT_AND_LOG(map_x + x < SCREEN_WIDTH, "map_x+x=%i, map_x=%i, x=%i\n", map_x+x, map_x, x);
            int __map_x = (map_x+x) - (SCREEN_WIDTH * buf_idx);
            map_segment_x = (uint8_t)(__map_x/(SCREEN_WIDTH/4)) % 4;
            map_segment_y = (uint8_t)(map_y/(SCREEN_HEIGHT/4)) % 4;
            SDL_Rect sprite_rect = GetSpriteRect(map_idx_buffer[buf_idx], map_segment_y, map_segment_x);

            int strip_w = BG_SPRITE_WIDTH - tile_offset_x;
            // Handle the edge-case near the end of the strip.
            if (end_x - x == BG_SPRITE_WIDTH && _tile_offset_x > 0) {
                strip_w = BG_SPRITE_WIDTH - _tile_offset_x;
            }

            int src_x       = sprite_rect.x + tile_offset_x;
            int grass_src_x = grass_rect.x  + tile_offset_x;
            int src_y       = sprite_rect.y + tile_offset_y;
            int grass_src_y = grass_rect.y  + tile_offset_y;

            SDL_Rect src_strip =   {src_x,       src_y,            strip_w, STEP_SIZE};
            SDL_Rect grass_strip = {grass_src_x, grass_src_y,      strip_w, STEP_SIZE};
            SDL_Rect dst_strip =   {x,           blit_placement_y, strip_w, STEP_SIZE};

            assert(src_x % BG_SPRITE_WIDTH  == grass_src_x % BG_SPRITE_WIDTH);
            assert(src_y % BG_SPRITE_HEIGHT == grass_src_y % BG_SPRITE_HEIGHT);
            assert(src_x >= 0);
            assert(src_y >= 0);

            if (tile_offset_x > 0) {
                x -= tile_offset_x;
                tile_offset_x = 0;
            }

            int v =      MAPS[map_idx_buffer[buf_idx]-1][map_segment_y][map_segment_x];
            if (v == -1) SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);
            else {
                         SDL_RenderCopy(_renderer, spritesheet, &grass_strip, &dst_strip);    // Render the grass first
                         SDL_RenderCopy(_renderer, spritesheet, &src_strip,   &dst_strip);    // Overlay the actual sprite
            }
        }
    } else if (_player_direction == PLAYER_DIRECTION::LEFT || _player_direction == PLAYER_DIRECTION::RIGHT) {
        for (int y = 0; y < end_y; y+=BG_SPRITE_HEIGHT) {
            if (map_y + y == SCREEN_HEIGHT) { buf_idx++; }              // If during horizontal strip, we move to a next map
            if (buf_idx == 0) assert(map_y + y < SCREEN_WIDTH);

            // keep the iterated y state in-tact but we might've moved into grabbing pixels from the next map.
            int __map_y = (map_y+y) - (SCREEN_HEIGHT * buf_idx);
            map_segment_x = (uint8_t)(map_x/(SCREEN_WIDTH/4)) % 4;
            map_segment_y = (uint8_t)(__map_y/(SCREEN_HEIGHT/4)) % 4;

            SDL_Rect sprite_rect = GetSpriteRect(map_idx_buffer[buf_idx], map_segment_y, map_segment_x);

            int strip_h = BG_SPRITE_HEIGHT - tile_offset_y;
            // Handle the edge-case near the end of the strip.
            if (end_y - y == BG_SPRITE_HEIGHT && _tile_offset_y > 0) {
                strip_h = BG_SPRITE_HEIGHT - _tile_offset_y;
            }

            int src_x       = sprite_rect.x + tile_offset_x;
            int grass_src_x = grass_rect.x  + tile_offset_x;
            int src_y       = sprite_rect.y + tile_offset_y;
            int grass_src_y = grass_rect.y  + tile_offset_y;

            SDL_Rect src_strip =   {src_x,             src_y,       STEP_SIZE, strip_h};
            SDL_Rect grass_strip = {grass_src_x,       grass_src_y, STEP_SIZE, strip_h};
            SDL_Rect dst_strip =   {blit_placement_x,  y,           STEP_SIZE, strip_h};

            assert(src_x % BG_SPRITE_WIDTH  == grass_src_x % BG_SPRITE_WIDTH);
            assert(src_y % BG_SPRITE_HEIGHT == grass_src_y % BG_SPRITE_HEIGHT);
            assert(src_x >= 0);
            assert(src_y >= 0);

            if (tile_offset_y > 0) {
                y -= tile_offset_y;
                tile_offset_y = 0;
            }

            int v =      MAPS[map_idx_buffer[buf_idx]-1][map_segment_y][map_segment_x];
            if (v == -1) SDL_RenderCopy(_renderer, spritesheet, &src_strip, &dst_strip);
            else {
                         SDL_RenderCopy(_renderer, spritesheet, &grass_strip, &dst_strip);    // Render the grass first
                         SDL_RenderCopy(_renderer, spritesheet, &src_strip,   &dst_strip);    // Overlay the actual sprite
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

    }
    DrawPlayerBoundingBox(); // Temporarily draw the bounding box.

    for (; i < current_scene->textures.size(); ++i) {
        SDL_Texture* texture = current_scene->textures[i];
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        if (_common->isTextTexture(tag)) {
            // FIXME: This is a hack, we want to first draw the quad and then the text.
            // Menu quad. (optional)
            if (_game_state == game_state_t::PAUSE) {
                int pause_x = SCREEN_WIDTH * 0.35;
                int pause_y = SCREEN_HEIGHT * 0.25;
                int pause_w = SCREEN_WIDTH * 0.3;
                int pause_h = SCREEN_HEIGHT * 0.4;
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
                if (begin_x > minimap_end_x) continue;
                if (begin_y > minimap_end_y) continue;
                DrawSquare(begin_x, begin_y, SCREEN_WIDTH*0.1 / 4, SCREEN_HEIGHT*0.1 / 4 *(4/3), minimap_end_x, minimap_end_y);
            }
        }
    }

    // Reset the color
    SDL_SetRenderDrawColor( _renderer, 0x00, 0x00, 0x00, 0xFF );

    if (_scene_stack_idx == 1) {
        // Cleanup temporary textures.
       uint8_t textures_to_clean = current_scene->textures.size() - _common->GetInitialSceneTextureSize();
       uint8_t i = textures_to_clean;
       while(i > 0) {
           SDL_DestroyTexture(current_scene->textures[_common->GetInitialSceneTextureSize() - 1 + i]);
           current_scene->textures.pop_back();
           current_scene->texture_src_rects.pop_back();
           current_scene->texture_dst_rects.pop_back();
           current_scene->tags.pop_back();
           i--;
       }

       LOG_nodt(1, "PERF", "FPS: %i\n", _fps);

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
    _prev_player_direction = _player_direction;
    _player_direction = PLAYER_DIRECTION::UP;
    DIRECTION();
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
    _prev_player_direction = _player_direction;
    _player_direction = PLAYER_DIRECTION::DOWN;
    DIRECTION();
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
    _prev_player_direction = _player_direction;
    _player_direction = PLAYER_DIRECTION::LEFT;
    DIRECTION();
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
    _prev_player_direction = _player_direction;
    _player_direction = PLAYER_DIRECTION::RIGHT;
    DIRECTION();
    _scroll_x += STEP_SIZE;
    _player_x += STEP_SIZE;
    UpdateMap();
    UpdateCamera();
    BlitNext();
}

void Game::HandleEscKey() {
    LOG_INFO("Escape key pressed\n");
    if (_game_state == game_state_t::PAUSE) _game_state = game_state_t::PLAY;
    else _game_state = game_state_t::PAUSE;
}

static std::shared_ptr<Common> common_ptr;
static std::unique_ptr<Game> game;

void initialize_the_game() {
    LOG(1, "TRACE", "initialize_the_game()\n");
    std::string app_name = std::string("Game");
    common_ptr = std::make_shared<Common>(std::move(app_name), BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
    LOG_INFO("Able to construct Common\n");
    game = std::make_unique<Game>(common_ptr);
    LOG_INFO("Able to construct Game\n");
    Uint64 before = SDL_GetTicks64();
    game->FillBackBufferInitial();
    LOG_INFO("Fill back buffer initial took: %zums\n", SDL_GetTicks64()-before);
}

static Uint64 tick = SDL_GetTicks64();     // SDL Library is initialized above via Common::Common()
static Uint64 last_frame_tick = tick - 17; // Ensures we always render the first frame straight-away.
static Uint64 dt_frame = 17;               // Ensures no initial event wait.
static Uint64 frame_per_second = 0;        // Counter that gets reset after 1s.
static Uint64 seconds = 1;                 // Seconds rendered.

static void mainloop(void) {
    int quit_app = 0;

    SDL_Event eh;

    int DEFAULT_WAIT = 13;
    int timeout = 0;

    bool interactive = true;

    if (dt_frame < DEFAULT_WAIT && timeout != 0) timeout = DEFAULT_WAIT - dt_frame; // Allow 3ms to draw or handle interaction.

    while (SDL_WaitEventTimeout(&eh, timeout) != 0) {
        if (eh.type == SDL_QUIT) {
            // Cleanup all current scene textures.
            auto current_scene = common_ptr->GetCurrentScene();
            uint8_t textures_to_clean = current_scene->textures.size();
            uint8_t i = textures_to_clean;
            while(i > 0) {
                SDL_DestroyTexture(current_scene->textures[common_ptr->GetInitialSceneTextureSize() - 1 + i]);
                current_scene->textures.pop_back();
                current_scene->texture_src_rects.pop_back();
                current_scene->texture_dst_rects.pop_back();
                current_scene->tags.pop_back();
                i--;
            }

            TTF_Quit();
            SDL_Quit();
            exit(1);
        }
        if (eh.type == SDL_KEYDOWN) {
            SDL_KeyboardEvent key = eh.key;
            SDL_Keysym keysym = key.keysym;

            switch(keysym.sym) {
            case SDLK_SPACE:
                game->HandleSpaceKey();
                interactive = true;
                break;
            case SDLK_UP:
                game->HandleUpKey();
                interactive = true;
                game->SetPrevActionInfoKey(SDLK_UP);
                break;
            case SDLK_DOWN:
                game->HandleDownKey();
                interactive = true;
                game->SetPrevActionInfoKey(SDLK_DOWN);
                break;
            case SDLK_LEFT:
                game->HandleLeftKey();
                interactive = true;
                game->SetPrevActionInfoKey(SDLK_LEFT);
                break;
            case SDLK_RIGHT:
                game->HandleRightKey();
                interactive = true;
                game->SetPrevActionInfoKey(SDLK_RIGHT);
                break;
            case SDLK_ESCAPE:
                game->HandleEscKey();
                interactive = true;
                break;
            }
        }
        if (eh.type == SDL_KEYUP) { game->SetPlayerAction(PLAYER_ACTION::STOPPED); }
    }

    #ifdef __EMSCRIPTEN__
    if (interactive) game->UpdateBackBuffer();
    #else
    tick = SDL_GetTicks64();
    dt_frame = tick - last_frame_tick;
    if (dt_frame > DEFAULT_WAIT && interactive) { // Allow 2ms draw time.
        game->set_fps(frame_per_second++);
        Uint64 before = SDL_GetTicks64();
        game->UpdateBackBuffer();
        Uint64 after = SDL_GetTicks64();
        interactive = false;
        LOG(1, "PERF", "Updated back buffer in %zu ms\n", after-before);
        LOG(1, "PERF", "Tick: %zu\n", tick);
        LOG(1, "PERF", "Seconds rendered: %zu\n", seconds);
        if (tick > 1000 * seconds) {
            LOG(1, "PERF", ">1 second in tick\n");
            seconds++;
            frame_per_second=0;
        }
        frame_count++;
        last_frame_tick = tick;
        dt_frame = SDL_GetTicks64() - after;
    }
    #endif
}

int main() {
    initialize_the_game();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 60, 1);
    #else
    while (1) { mainloop(); }
    #endif
    return 0;
}

// TODO: Going south has an off by STEP_SIZE issue.
// TODO: Mixing up/down with left/right leads to incorrect offsets somehow.




// TODO: Draw minimap.
// TODO: Menu on ESC.
// TODO: Save game.
// TODO: First use player sprite as NPC to test out dialog.
