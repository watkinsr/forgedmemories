#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <memory>
#include "Log.h"
#include "Common.h"

using namespace std;

enum PLAYER_ACTION {
    STOPPED,
    MOVING,
    ATTACK
};

enum game_state_t {
    PLAY,
    PAUSE
};


enum PLAYER_DIRECTION {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

#define STEP_SIZE 10

#define SPRITE_SCALE_FACTOR 3.125
#define BG_SPRITE_WIDTH 50
#define BG_SPRITE_HEIGHT 50

const int SPRITE_WIDTH = 50;
const int SPRITE_HEIGHT = 50;

const uint8_t MOVE_ANIM_TICKS = 16;
const uint16_t ATTACK_ANIMATION_FRAMES = 6 * 3;

const int BACKBUFFER_WIDTH = 800;
const int BACKBUFFER_HEIGHT = 600;

struct attack_animation_t {
    uint8_t runtime;
    bool active;
    uint16_t x;
};

struct move_animation_t {
    uint8_t remaining_ticks = 0;
    player_direction_t direction = player_direction_t::DOWN;
};

class Game {
public:
    Game(std::shared_ptr<Common> common_ptr);
    ~Game();
    void SetPlayerState(const player_state_t state) { _player_state = state; };
    void FillBackBufferInitial();
    void UpdateBackBuffer();
    void SetPlayerY(const int32_t y) { _player_y = y; };
    const int32_t GetPlayerY() { return _player_y; };
    void SetPlayerX(const int32_t x) { _player_x = x; };
    void SetPlayerAction(const PLAYER_ACTION action) { _player_action = action; };
    void SetAttackAnimation(const uint8_t runtime, const bool active) {
        if (_attack_animation.runtime > 0) return;
        _attack_animation.runtime = runtime;
        _attack_animation.active = active;
        _attack_animation.x = PLAYER_BEGIN_X;
    };
    void SetPlayerDirection(const PLAYER_DIRECTION direction) { _player_direction = direction; };
    const int32_t GetPlayerX() { return _player_x; };
    bool AfterMainMenu();
    bool IsColliding(const int, const int);
    void UpdateMap();
    uint8_t GetCenterIdx(uint8_t);
    uint8_t GetEastIdx(uint8_t);
    uint8_t GetWestIdx(uint8_t);
    uint8_t GetSouthIdx(uint8_t);
    uint8_t GetNorthIdx(uint8_t);
    uint8_t GetNorthEastIdx(uint8_t);
    uint8_t GetNorthWestIdx(uint8_t);
    uint8_t GetSouthEastIdx(uint8_t);
    uint8_t GetSouthWestIdx(uint8_t);
    void set_fps(const uint8_t fps) { _fps = fps; };
    void RenderSprite(SDL_Rect&, SDL_Rect&, const uint8_t, const uint8_t, const uint8_t, SDL_Texture&);
    void ResetAttackAnimation();
    const bool AttackAnimationActive() { return _attack_animation.active; }
    void DrawCircle(int, int, int);
    void DrawSquare(const int, const int, const int, const int, const int, const int);
    void DrawPlayerBoundingBox();
    void ResetMoveAnimation();
    void TickPlayerMove() { _move_animation.remaining_ticks--; };
    void HandleEscKey();
    void HandleSpaceKey();
    void HandleUpKey();
    void HandleLeftKey();
    void HandleRightKey();
    void HandleDownKey();
    void BlitNext();
private:
    PLAYER_ACTION _player_action = PLAYER_ACTION::STOPPED;
    PLAYER_DIRECTION _player_direction = PLAYER_DIRECTION::DOWN;
    void _SetTextureLocations();
    uint64_t _tick;
    uint32_t _deltaTick;
    const uint32_t PLAYER_BEGIN_X = SCREEN_WIDTH/2 - PLAYER_WIDTH/2;
    const uint32_t PLAYER_BEGIN_Y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    int32_t _player_x = PLAYER_BEGIN_X;
    int32_t _player_y = PLAYER_BEGIN_Y;

    int32_t _scroll_x = 0;    // Should be sufficient.
    int32_t _scroll_y = 0;   // Should be sufficient.
    int cached_scroll_y = _scroll_y;
    int cached_scroll_x = _scroll_x;

    SDL_Rect grass_rect = {SPRITE_WIDTH, SPRITE_HEIGHT*4, BG_SPRITE_WIDTH, BG_SPRITE_HEIGHT};

    attack_animation_t _attack_animation = {0, false, PLAYER_BEGIN_X};
    move_animation_t _move_animation = {0};
    std::shared_ptr<Common> _common;
    game_state_t _game_state = game_state_t::PLAY;
    uint8_t _fps = 0;
};

#endif
