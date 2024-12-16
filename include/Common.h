#ifndef COMMON_H
#define COMMON_H

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>
#include "Log.h"

using namespace std;

constexpr uint8_t DEFAULT_FONT_ARRAY_LEN = 2;
const uint32_t SCREEN_WIDTH = 800;
const uint32_t SCREEN_HEIGHT = 600;

#define TEXT_TAG 1 << 0
#define IMAGE_TAG 1 << 1
#define SPRITE_TAG 1 << 2
#define RECT_TAG 1 << 3

#define PLAYER_SPRITE_FLAG 1 << 0
#define BACKGROUND_SPRITE_FLAG 1 << 1
#define ENEMY_SPRITE_FLAG 0x03

struct scene_t {
    vector<SDL_Texture*> textures;
    vector<uint8_t> tags;
    vector<SDL_Rect> texture_src_rects;
    vector<SDL_Rect> texture_dst_rects;
    vector<SDL_Color> colors;
};

struct gametexture_t {
    string text_or_uri;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
    uint8_t font_size;
    uint8_t tag;
};

constexpr uint8_t PLAYER_WIDTH = 48;
constexpr uint8_t PLAYER_HEIGHT = 48;

class Common {
public:
    Common(std::string app_name);
    ~Common();
    void SetupSDL();
    enum FONT_SIZE {
        SMALL,
        MEDIUM,
        LARGE
    };
    constexpr static std::array<std::string_view, 2> DEFAULT_FONTS = {
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf"
    };
    void AddScene(std::vector<gametexture_t>);
    SDL_Event* GetEvent();
    void AllocateScene(bool);
    void LoadTexture(const uint8_t, gametexture_t);
    bool isEnemySpriteTexture(uint8_t);
    bool isPlayerSpriteTexture(uint8_t);
    bool isBackgroundSpriteTexture(uint8_t);
    bool isTextTexture(uint8_t);
    bool isImageTexture(uint8_t);
    bool isSpriteTexture(uint8_t);
    bool isRectTexture(uint8_t);
    std::pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderCurrentScene();
    SDL_Renderer* GetRenderer();
    SDL_Window* GetWindow();
    uint8_t GetSceneStackIdx();
    scene_t* GetCurrentScene();
    void SetInitialSceneTextureSize(const uint8_t);
    const uint8_t GetInitialSceneTextureSize();
private:
    std::function<void(TTF_Font*)> fontDeleter;
    const uint8_t  SCENE_STACK_MAX_SIZE = 2;
    void _SetTextureLocations();
    std::string _app_name;
    uint32_t _tick;
    uint32_t _deltaTick;
    SDL_Window*                                                       _window;
    SDL_Renderer*                                                     _renderer;
    vector<std::unique_ptr<TTF_Font, std::function<void(TTF_Font*)>>> _fonts;
    SDL_Surface*                                                      _screen_surface;
    uint8_t                                                           _scene_stack_idx = 0;
    vector<SDL_Texture*>                                              _textures = vector<SDL_Texture*>();
    vector<scene_t>                                                   _scenes = vector<scene_t>();
    vector<vector<gametexture_t>>                                     _scene_texture_locations = vector<vector<gametexture_t>>();
    uint8_t                                                           _initial_scene_size = 0;
};

#endif
