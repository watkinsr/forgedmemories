#include "Common.h"
#include "MapEditor.h"

void MapEditor::RenderCurrentScene() {
    // LOG_INFO("MapEditor::RenderCurrentScene()");
    SDL_Renderer* _renderer = _common->GetRenderer();
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();
    float tick = SDL_GetTicks();
    if (_scene_stack_idx == 0) SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    else SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);
    uint8_t texture_idx = 0;
    for (uint8_t i = 0; i < current_scene->texture_src_rects.size(); ++i) {
        // LOG_INFO("Current texture: %i", i);
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        // LOG_INFO("Tag: %i", tag);
        if (_common->isRectTexture(tag)) {
            SDL_SetRenderDrawColor(_renderer, 128, 128, 128, 255);
            SDL_RenderFillRect(_renderer, &src_rect);
        }
        if (_common->isTextTexture(tag)) {
            SDL_Texture* texture = current_scene->textures[texture_idx++];
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        }
    }
    SDL_RenderPresent(_renderer);
}

void MapEditor::_SetTextureLocations() {
    uint16_t y = static_cast<float>(SCREEN_HEIGHT)*0.1f;
    const vector<gametexture_t> SCENE_1 = {
        { .text_or_uri = "",
          .src_rect = {0, 0, SCREEN_WIDTH, y},
          .dst_rect = {0, 0, SCREEN_WIDTH, y},
          .color = {128,128,128,255},
          .tag = RECT_TAG
      },
        { .text_or_uri = "File",
          .src_rect = {25, 10, 0, y},
          .dst_rect = {25, 10, 0, y},
          .color = {255,255,255,255},
          .tag = TEXT_TAG
      },
    };
    _common->AddScene(SCENE_1);
}

MapEditor::MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    _SetTextureLocations();
    common_ptr->AllocateScene(false);
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
