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
    vector<SDL_Rect> texture_rects;
};

struct gametexture_t {
    string text_or_uri;
    SDL_Rect rect;
    SDL_Color color;
    uint8_t tag;
};

class Game {
public:
    Game();
    ~Game();
    SDL_Event* GetEvent();
    void LoadTexture(const uint8_t, gametexture_t);
    pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderScene();
    void AllocateScene(bool);
private:
    void _SetTextureLocations();
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
    const uint8_t  MAIN_TEXTURE_SIZE = 2;
    const uint8_t  SCENE_STACK_MAX_SIZE = 2;
};

#endif
