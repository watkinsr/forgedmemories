#include "Game.h"
#include <memory>

void handleSpaceKey(std::unique_ptr<Game>& game) {
    game->AllocateScene(true);
}

void handleUpKey(std::unique_ptr<Game>& game) {
    game->SetPlayerY(game->GetPlayerY() - 1);
}

void handleDownKey(std::unique_ptr<Game>& game) {
    game->SetPlayerY(game->GetPlayerY() + 1);
}

void handleLeftKey(std::unique_ptr<Game>& game) {
    game->SetPlayerX(game->GetPlayerX() - 1);
}

void handleRightKey(std::unique_ptr<Game>& game) {
    game->SetPlayerX(game->GetPlayerX() + 1);
}

int main() {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    SDL_Event* e = game->GetEvent();
    while (e->type != SDL_QUIT){
        switch(e->type) {
            case SDL_KEYDOWN:
                if (e->key.keysym.sym == SDLK_SPACE) {
                    printf("Detected keydown on <SPC>\n");
                    handleSpaceKey(game);
                } else if (e->key.keysym.sym == SDLK_UP) {
                    handleUpKey(game);
                } else if (e->key.keysym.sym == SDLK_DOWN) {
                    handleDownKey(game);
                } else if (e->key.keysym.sym == SDLK_LEFT) {
                    handleLeftKey(game);
                } else if (e->key.keysym.sym == SDLK_RIGHT) {
                    handleRightKey(game);
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
