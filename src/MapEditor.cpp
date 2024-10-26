#include "Common.h"
#include "MapEditor.h"

void MapEditor::RenderCurrentScene() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();
    float tick = SDL_GetTicks();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    else SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
}

int main() {
    std::shared_ptr<Common> common_ptr = std::make_shared<Common>("Map Editor");
    std::unique_ptr<MapEditor> map_editor = std::make_unique<MapEditor>(common_ptr);
    SDL_Event* e = common_ptr->GetEvent();
    while (e->type != SDL_QUIT){
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_SPACE]) {
            // handleSpaceKey(common_ptr, game);
        } else if (state[SDL_SCANCODE_UP]) {
            // handleUpKey(game);
        } else if (state[SDL_SCANCODE_DOWN]) {
            // handleDownKey(game);
        } else if (state[SDL_SCANCODE_LEFT]) {
            // handleLeftKey(game);
        } else if (state[SDL_SCANCODE_RIGHT]) {
            // handleRightKey(game);
        } else {
            // game->SetPlayerState(player_state_t::STOPPED);
        }

        map_editor->RenderCurrentScene();
        e = common_ptr->GetEvent();
    }
    return 0;
}
