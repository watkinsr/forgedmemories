#ifndef GAME_H
#define GAME_H

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

struct scene_t {
    std::vector<SDL_Texture*> textures;
    std::vector<SDL_Rect> texture_rects;
};

class Game {
public:
    Game();
    ~Game();
    SDL_Event* GetEvent();
    void LoadTexture(const uint8_t, const std::string path, const uint32_t, const uint32_t, const uint32_t, const uint32_t);
    void AllocateTextTexture(const uint8_t, const SDL_Color, const std::string, const uint32_t, const uint32_t);
    std::pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderScene();
    void IncrementSceneStack();
    void InitDefaultScene();
private:
    SDL_Window*               _window;
    SDL_Renderer*             _renderer;
    SDL_Event*                _event;
    TTF_Font*                 _font;
    SDL_Surface*              _main_menu_play_surface;
    uint8_t                   _scene_stack_idx = 0;
    std::vector<SDL_Texture*> _textures = std::vector<SDL_Texture*>();
    std::vector<scene_t> _scenes = std::vector<scene_t>();
    const uint32_t SCREEN_WIDTH = 640;
    const uint32_t SCREEN_HEIGHT = 480;
    const uint8_t  MAIN_TEXTURE_SIZE = 2;
    const uint8_t  SCENE_STACK_MAX_SIZE = 1;
};

#endif
