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
#include <memory>

#include "Log.h"

using namespace std;

enum FONT_SIZE {
    SMALL,
    MEDIUM,
    LARGE
};

constexpr uint8_t DEFAULT_FONT_ARRAY_LEN = 2;
const ssize_t SCREEN_WIDTH = 800;
const ssize_t SCREEN_HEIGHT = 600;

constexpr uint16_t SPRITESHEET_WIDTH = 256;   // Technically only 128 is actually sprites.
constexpr uint16_t SPRITESHEET_HEIGHT = 256;

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

struct upscale_t {
    int w = 0;
    int h = 0;
};

struct gametexture_t {
    string text_or_uri;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
    uint8_t font_size = FONT_SIZE::SMALL;
    uint8_t tag;
    uint8_t idx;  // DEBUG PURPOSE
    upscale_t upscale;
};

class Common {
public:
    Common(std::string app_name, const uint32_t, const uint32_t);
    ~Common();
    void SetupSDL();

    constexpr static std::array<std::string_view, 3> DEFAULT_FONTS = {
        "/usr/share/fonts/gnu-free/FreeMono.ttf",
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
    SDL_Renderer* GetRenderer();
    SDL_Texture* GetBackBuffer() { return _back_buffer; };
    void SetBackBuffer(SDL_Texture* t) { _back_buffer = t; };
    SDL_Window* GetWindow();
    uint8_t GetSceneStackIdx();
    scene_t* GetCurrentScene();
    void SetInitialSceneTextureSize(const uint8_t);
    uint8_t GetInitialSceneTextureSize();
    void DestroyFonts();
private:
    uint32_t _BACKBUFFER_WIDTH;
    uint32_t _BACKBUFFER_HEIGHT;
    std::function<void(TTF_Font*)> fontDeleter;
    const uint8_t  SCENE_STACK_MAX_SIZE = 2;
    void _SetTextureLocations();
    const char* _app_name;
    uint32_t _tick;
    uint32_t _deltaTick;
    SDL_Texture*                                                      _back_buffer;
    SDL_Window*                                                       _window;
    SDL_Renderer*                                                     _renderer;
    vector<std::shared_ptr<TTF_Font>>                                 _fonts = {};
    SDL_Surface*                                                      _screen_surface;
    uint8_t                                                           _scene_stack_idx = 0;
    vector<scene_t>                                                   _scenes = vector<scene_t>();
    vector<vector<gametexture_t>>                                     _scene_texture_locations = vector<vector<gametexture_t>>();
    uint8_t                                                           _initial_scene_size = 0;
};

#define DIRECTION()                                             \
    do {                                                        \
        if (_player_direction == PLAYER_DIRECTION::UP) {        \
            fprintf(stderr, ", d=UP");                          \
        }                                                       \
        if (_player_direction == PLAYER_DIRECTION::DOWN) {      \
            fprintf(stderr, ", d=RIGHT");                       \
        }                                                       \
        if (_player_direction == PLAYER_DIRECTION::LEFT) {      \
            fprintf(stderr, ", d=LEFT");                        \
        }                                                       \
        if (_player_direction == PLAYER_DIRECTION::RIGHT) {     \
            fprintf(stderr, ", d=RIGHT");                       \
        }                                                       \
        fprintf(stderr, "\n");                                  \
    } while (0)

#define ASSERT_AND_LOG(cond, fmt, ...)                          \
    do {                                                        \
        if (!(cond)) {                                          \
            fprintf(stderr,                                     \
                    "[%s:%d] Assertion failed: %s\n",           \
                    __FILE__, __LINE__, #cond);                 \
            fprintf(stderr, fmt, ##__VA_ARGS__);                \
            DIRECTION();                                        \
            fprintf(stderr, "\n");                              \
            abort();                                            \
        }                                                       \
    } while (0)

#endif
