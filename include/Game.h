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

enum player_direction_t {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Game {
public:
    Game(std::shared_ptr<Common> common_ptr);
    ~Game();
    void RenderCurrentScene();
    void SetPlayerY(const int32_t y) { _player_y = y; };
    const int32_t GetPlayerY() { return _player_y; };
    void SetPlayerX(const int32_t x) { _player_x = x; };
    void SetPlayerState(const player_state_t state) { _player_state = state; };
    void SetPlayerDirection(const player_direction_t direction) { _player_direction = direction; };
    const int32_t GetPlayerX() { return _player_x; };
    const bool AfterMainMenu();
    const bool IsColliding(uint16_t, uint16_t);
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
private:
    player_state_t _player_state = player_state_t::STOPPED;
    player_direction_t _player_direction = player_direction_t::DOWN;
    void _SetTextureLocations();
    uint32_t _tick;
    uint32_t _deltaTick;
    const uint32_t PLAYER_BEGIN_X = SCREEN_WIDTH/2 - PLAYER_WIDTH/2;
    const uint32_t PLAYER_BEGIN_Y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    int32_t _player_x = PLAYER_BEGIN_X;
    int32_t _player_y = PLAYER_BEGIN_Y;
    std::shared_ptr<Common> _common;
};

#endif
