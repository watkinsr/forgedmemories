#include "Game.h"
#include <memory>

#define STEP_SIZE 3

void handleSpaceKey(std::unique_ptr<Game>& game) {
    if (!game->AfterMainMenu()) game->AllocateScene(true);
    else {
        game->SetPlayerState(player_state_t::ATTACK);
    }
}

void handleUpKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX(),
        game->GetPlayerY() - STEP_SIZE)) return;
    game->SetPlayerY(game->GetPlayerY() - STEP_SIZE);
    game->SetPlayerState(player_state_t::MOVING);
    game->UpdateMap();
}

void handleDownKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX(),
        game->GetPlayerY() + STEP_SIZE)) return;
    game->SetPlayerY(game->GetPlayerY() + STEP_SIZE);
    game->SetPlayerState(player_state_t::MOVING);
    game->UpdateMap();
}

void handleLeftKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX() - STEP_SIZE,
        game->GetPlayerY())) return;
    game->SetPlayerX(game->GetPlayerX() - STEP_SIZE);
    game->SetPlayerState(player_state_t::MOVING);
    game->UpdateMap();
}

void handleRightKey(std::unique_ptr<Game>& game) {
    if (game->IsColliding(
        game->GetPlayerX() + STEP_SIZE,
        game->GetPlayerY())) return;
    game->SetPlayerX(game->GetPlayerX() + STEP_SIZE);
    game->SetPlayerState(player_state_t::MOVING);
    game->UpdateMap();
}

int main() {
    std::unique_ptr<Game> game = std::make_unique<Game>();
    SDL_Event* e = game->GetEvent();
    while (e->type != SDL_QUIT){
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_SPACE]) {
            handleSpaceKey(game);
        } else if (state[SDL_SCANCODE_UP]) {
            handleUpKey(game);
        } else if (state[SDL_SCANCODE_DOWN]) {
            handleDownKey(game);
        } else if (state[SDL_SCANCODE_LEFT]) {
            handleLeftKey(game);
        } else if (state[SDL_SCANCODE_RIGHT]) {
            handleRightKey(game);
        } else {
            game->SetPlayerState(player_state_t::STOPPED);
        }

        game->RenderScene();
        e = game->GetEvent();
    }
    return 0;
}
