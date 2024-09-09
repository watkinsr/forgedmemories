#ifndef GAME_H
#define GAME_H

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class Game {
public:
    Game();
    ~Game();
    SDL_Event* GetEvent();
    void LoadTexture(std::string);
    void AllocateTextTexture(SDL_Color, std::string);
    std::pair<int, int> GetTextureDimensions(SDL_Texture*);
    void RenderScene();
private:
    SDL_Window*               _window;
    SDL_Renderer*             _renderer;
    SDL_Event*                _event;
    TTF_Font*                 _font;
    SDL_Surface*              _main_menu_play_surface;
    std::vector<SDL_Texture*> _textures = std::vector<SDL_Texture*>();
    const uint32_t SCREEN_WIDTH = 640;
    const uint32_t SCREEN_HEIGHT = 480;
};

#endif
