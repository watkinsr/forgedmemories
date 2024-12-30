#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <memory>
#include "Log.h"
#include "Common.h"

using namespace std;

enum player_state_t {
    STOPPED,
    MOVING,
    ATTACK
};

enum game_state_t {
    PLAY,
    PAUSE
};

enum player_direction_t {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

#define STEP_SIZE 5
const uint8_t MOVE_ANIM_TICKS = 16;
const uint16_t ATTACK_ANIMATION_FRAMES = 6 * 3;
const float SPRITE_SCALE_FACTOR = 3.125;

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
    void RenderCurrentScene();
    void SetPlayerY(const int32_t y) {
        // LOG_INFO("Y: %i", y);
        _player_y = y;
    };
    const int32_t GetPlayerY() { return _player_y; };
    void SetPlayerX(const int32_t x) {
        // LOG_INFO("X: %i", x);
        _player_x = x;
    };
    void SetPlayerState(const player_state_t state) { _player_state = state; };
    void SetAttackAnimation(const uint8_t runtime, const bool active) {
        if (_attack_animation.runtime > 0) return;
        _attack_animation.runtime = runtime;
        _attack_animation.active = active;
        _attack_animation.x = PLAYER_BEGIN_X;
    };
    void SetPlayerDirection(const player_direction_t direction) { _player_direction = direction; };
    const int32_t GetPlayerX() { return _player_x; };
    const bool AfterMainMenu();
    const bool IsColliding(const int, const int);
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
private:
    player_state_t _player_state = player_state_t::STOPPED;
    player_direction_t _player_direction = player_direction_t::DOWN;
    void _SetTextureLocations();
    uint64_t _tick;
    uint32_t _deltaTick;
    const uint32_t PLAYER_BEGIN_X = SCREEN_WIDTH/2 - PLAYER_WIDTH/2;
    const uint32_t PLAYER_BEGIN_Y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    int32_t _player_x = PLAYER_BEGIN_X;
    int32_t _player_y = PLAYER_BEGIN_Y;
    attack_animation_t _attack_animation = {0, false, PLAYER_BEGIN_X};
    move_animation_t _move_animation = {0};
    std::shared_ptr<Common> _common;
    game_state_t _game_state = game_state_t::PLAY;
    uint8_t _fps = 0;
};

#endif
