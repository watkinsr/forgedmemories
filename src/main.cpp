#include "Game.h"
#include <memory>

int main() {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->LoadTexture("assets/menu.gif");
    game->AllocateTextTexture({255,255,255,255}, "Play");
    SDL_Event* e = game->GetEvent();
    while (e->type != SDL_QUIT){
        switch(e->type) {
            case SDL_KEYDOWN:
                if (e->key.keysym.sym == SDLK_SPACE) {
                    printf("Detected keydown on <SPC>\n");
                }
                break;
            default:
                break;
	    }

        game->RenderScene();
        e = game->GetEvent();
	}
	return 0;
}
