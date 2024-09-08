#include "Game.h"
#include <memory>

int main() {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->LoadTexture("assets/menu.gif");
    while (game->GetEvent()->type != SDL_QUIT){
        game->Draw();
	}
	return 0;
}
