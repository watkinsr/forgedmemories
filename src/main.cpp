#include "Game.h"
#include <memory>

void handleSpaceKey(std::unique_ptr<Game>& game) {
    game->IncrementSceneStack();
}

int main() {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->InitDefaultScene();
    SDL_Event* e = game->GetEvent();
    while (e->type != SDL_QUIT){
        switch(e->type) {
            case SDL_KEYDOWN:
                if (e->key.keysym.sym == SDLK_SPACE) {
                    printf("Detected keydown on <SPC>\n");
                    handleSpaceKey(game);
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
