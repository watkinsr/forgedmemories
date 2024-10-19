#ifndef GAME_H
#define GAME_H

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Log.h"

using namespace std;

struct scene_t {
    vector<SDL_Texture*> textures;
    vector<uint8_t> tags;
    vector<SDL_Rect> texture_src_rects;
    vector<SDL_Rect> texture_dst_rects;
};

struct gametexture_t {
    string text_or_uri;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
    uint8_t tag;
};

enum player_state_t {
    STOPPED,
    MOVING,
    ATTACK
};

constexpr uint8_t PLAYER_WIDTH = 48;
constexpr uint8_t PLAYER_HEIGHT = 48;

class Game {
public:
    Game();
    ~Game();
    SDL_Event* GetEvent();
    void LoadTexture(const uint8_t, gametexture_t);
    pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderScene();
    void AllocateScene(bool);
    void SetPlayerY(const int32_t y) { _player_y = y; };
    const int32_t GetPlayerY() { return _player_y; };
    void SetPlayerX(const int32_t x) { _player_x = x; };
    void SetPlayerState(const player_state_t state) { _player_state = state; };
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
    void _SetTextureLocations();
    uint32_t _tick;
    uint32_t _deltaTick;
    SDL_Window*               _window;
    SDL_Renderer*             _renderer;
    SDL_Event*                _event;
    TTF_Font*                 _font;
    uint8_t                   _scene_stack_idx = 0;
    vector<SDL_Texture*> _textures = vector<SDL_Texture*>();
    vector<scene_t> _scenes = vector<scene_t>();
    vector<vector<gametexture_t>> _scene_texture_locations = vector<vector<gametexture_t>>();
    const uint32_t SCREEN_WIDTH = 640;
    const uint32_t SCREEN_HEIGHT = 480;
    const uint32_t PLAYER_BEGIN_X = SCREEN_WIDTH/2 - PLAYER_WIDTH/2;
    const uint32_t PLAYER_BEGIN_Y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    int32_t _player_x = PLAYER_BEGIN_X;
    int32_t _player_y = PLAYER_BEGIN_Y;
    const uint8_t  MAIN_TEXTURE_SIZE = 2;
    const uint8_t  SCENE_STACK_MAX_SIZE = 2;
};

#endif
