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
    uint8_t color_idx = 0;
    for (uint8_t i = 0; i < current_scene->texture_src_rects.size(); ++i) {
        // LOG_INFO("Current texture: %i", i);
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
        // LOG_INFO("Tag: %i", tag);
        if (_common->isRectTexture(tag)) {
            SDL_Color color = current_scene->colors[color_idx++];
            SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(_renderer, &src_rect);
        } else if (_common->isTextTexture(tag)) {
            SDL_Texture* texture = current_scene->textures[texture_idx++];
            SDL_RenderCopy(_renderer, texture, NULL, &dst_rect);
        } else if (_common->isBackgroundSpriteTexture(tag)) {
            SDL_Texture* texture = current_scene->textures[texture_idx++];
            const uint16_t x_offset = SCREEN_WIDTH - 160;
            const uint16_t y_offset = 64;
            for (uint8_t i = 0; i < 4; ++i) {
                for (uint8_t j = 0; j < 8; ++j) {
                    dst_rect.x = x_offset + (i*16*2);
                    dst_rect.y = y_offset + (j*16*2);
                    dst_rect.w = 16 * 2;  // scale by 2.
                    dst_rect.h = 16 * 2;  // scale by 2.
                    src_rect.x = i*16;
                    src_rect.y = j*16;
                    SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
                }
            }

        }
    }
    SDL_RenderPresent(_renderer);
}

void MapEditor::_SetTextureLocations() {
    uint16_t y = static_cast<float>(SCREEN_HEIGHT)*0.1f;
    const vector<gametexture_t> SCENE_1 = {
        {   .text_or_uri = "",
            .src_rect = {0, 0, SCREEN_WIDTH, y},
            .dst_rect = {0, 0, SCREEN_WIDTH, y},
            .color = {128,128,128,255},
            .tag = RECT_TAG
        },
        {   .text_or_uri = "",
            .src_rect = {0, y*1, SCREEN_WIDTH-180, y * 9},
            .dst_rect = {0, y*1, SCREEN_WIDTH-180, y * 9},
            .color = {64,64,64,255},
            .tag = RECT_TAG
        },
        {   .text_or_uri = "File",
            .src_rect = {25, 10, 0, y},
            .dst_rect = {25, 10, 0, y},
            .color = {255,255,255,255},
            .tag = TEXT_TAG
        },
        {   .text_or_uri = "assets/bg/Berry Garden.png",
            .src_rect = {0, 0, 16, 16},
            .dst_rect = {0, 0, 64, 128},
            .color = {0,0,0,0},
            .tag = SPRITE_TAG | BACKGROUND_SPRITE_FLAG
        },
    };
    _common->AddScene(SCENE_1);
}

MapEditor::MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    _SetTextureLocations();
    common_ptr->AllocateScene(false);
}

void MapEditor::HandleSelection(const int mouse_x, const int mouse_y) {
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();
    LOG_INFO("Mouse X: %i, Mouse Y: %i", mouse_x, mouse_y);
    for (uint8_t i = 0; i < current_scene->texture_src_rects.size(); ++i) {
        uint8_t tag = current_scene->tags[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        const uint16_t x_offset = SCREEN_WIDTH - 160;
        const uint16_t y_offset = 64;
        if (_common->isBackgroundSpriteTexture(tag)) {
            for (uint8_t i = 0; i < 4; ++i) {
                for (uint8_t j = 0; j < 8; ++j) {
                    const uint16_t sprite_x = x_offset + (i*16*2);
                    const uint16_t sprite_y = y_offset + (j*16*2);
                    if (mouse_x > sprite_x && mouse_x <= sprite_x + 16 && mouse_y > sprite_y && mouse_y <= sprite_y + 16) {
                        LOG_INFO("Selection made on sprite.");
                    }
                }
            }
        }
    }
}

int main() {
    std::shared_ptr<Common> common_ptr = std::make_shared<Common>("Map Editor");
    std::unique_ptr<MapEditor> map_editor = std::make_unique<MapEditor>(common_ptr);
    SDL_Event* e = common_ptr->GetEvent();
    while (e->type != SDL_QUIT){
        const Uint8* state = SDL_GetKeyboardState(NULL);

        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState( &x, &y );
            map_editor->HandleSelection(x, y);
        }

        if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP ) {
            //
        }

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
