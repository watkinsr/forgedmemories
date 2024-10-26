#ifndef COMMON_H
#define COMMON_H

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Log.h"

using namespace std;

constexpr uint8_t DEFAULT_FONT_ARRAY_LEN = 2;
const uint32_t SCREEN_WIDTH = 640;
const uint32_t SCREEN_HEIGHT = 480;

#define TEXT_TAG 1 << 0
#define IMAGE_TAG 1 << 1
#define SPRITE_TAG 1 << 2

#define PLAYER_SPRITE_FLAG 1 << 0
#define BACKGROUND_SPRITE_FLAG 1 << 1
#define ENEMY_SPRITE_FLAG 0x03

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

constexpr uint8_t PLAYER_WIDTH = 48;
constexpr uint8_t PLAYER_HEIGHT = 48;

class Common {
public:
    Common(std::string app_name);
    ~Common();
    void SetupSDL();
    constexpr static std::array<std::string_view, 2> DEFAULT_FONTS = {
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf"
    };
    void AddScene(std::vector<gametexture_t>);
    SDL_Event* GetEvent();
    void AllocateScene(bool);
    void LoadTexture(const uint8_t, gametexture_t);
    constexpr bool isEnemySpriteTexture(uint8_t);
    constexpr bool isPlayerSpriteTexture(uint8_t);
    constexpr bool isBackgroundSpriteTexture(uint8_t);
    constexpr bool isTextTexture(uint8_t);
    constexpr bool isImageTexture(uint8_t);
    constexpr bool isSpriteTexture(uint8_t);
    std::pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderCurrentScene();
    SDL_Renderer* GetRenderer();
    uint8_t GetSceneStackIdx();
    scene_t* GetCurrentScene();
private:
    void _SetTextureLocations();
    std::string _app_name;
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
    const uint8_t  SCENE_STACK_MAX_SIZE = 2;
};

#endif
