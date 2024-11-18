#include "Common.h"
#include "MapEditor.h"

void MapEditor::RenderCurrentScene() {
    // LOG_INFO("MapEditor::RenderCurrentScene()");
    SDL_Renderer* _renderer = _common->GetRenderer();
    uint8_t _scene_stack_idx = _common->GetSceneStackIdx();
    scene_t* current_scene = _common->GetCurrentScene();

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
            // Get the spritesheet texture.
            SDL_Texture* texture = current_scene->textures[texture_idx++];

            // If there's a selection, we need to reflect drag and drop.
            if (_sprite_selection.selection) {
                src_rect.x = _sprite_selection.x*16;
                src_rect.y = _sprite_selection.y*16;
                src_rect.w = 16;
                src_rect.h = 16;
                dst_rect.x = _mouse_x;
                dst_rect.y = _mouse_y;
                dst_rect.w = 32;
                dst_rect.h = 32;
                SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
            }

            // HACK: Temporary to verify placement logic.
            if (_placement.x > 0) {
                src_rect.x = _sprite_selection.x*16;
                src_rect.y = _sprite_selection.y*16;
                src_rect.w = 16;
                src_rect.h = 16;
                dst_rect.x = _placement.x;
                dst_rect.y = _placement.y;
                dst_rect.w = 32;
                dst_rect.h = 32;
                SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
            }

            // Render the spritesheet to the right.
            const uint16_t x_offset = SCREEN_WIDTH - 160;
            const uint16_t y_offset = 64;

            src_rect.x = 0;
            src_rect.y = 0;
            src_rect.w = 256;
            src_rect.h = 256;
            dst_rect.x = x_offset;
            dst_rect.y = y_offset;
            dst_rect.w = 256;
            dst_rect.h = 256;

            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);

        }
    }
    SDL_RenderPresent(_renderer);

    LOG_INFO("Amount of textures in scene: %li", current_scene->textures.size());
    if (current_scene->textures.size() == 3) {
        // Remove the FPS texture.
        current_scene->textures.pop_back();
        current_scene->texture_src_rects.pop_back();
        current_scene->texture_dst_rects.pop_back();
    }

    float tick = SDL_GetTicks();
    float deltaTick = tick - _prev_tick;
    uint32_t fps = 1000.0f / deltaTick;
    gametexture_t fps_texture = {
        .text_or_uri = "FPS: " + std::to_string(fps),
        .src_rect = {0, 0, 0, 0},
        .dst_rect = {100, 5, 0, 0},
        .color = {255,255,255,255},
        .tag = TEXT_TAG
    };

    _prev_tick = tick;

    _common->LoadTexture(_scene_stack_idx, std::move(fps_texture));
}

void MapEditor::_SetTextureLocations() {
    float nav_y_factor = 0.05f;
    int nav_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(nav_y_factor);
    int playground_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(1.0f - nav_y_factor);
    const vector<gametexture_t> SCENE_1 = {
        {   .text_or_uri = "",
            .src_rect = {0, 0, SCREEN_WIDTH, nav_y},
            .dst_rect = {0, 0, SCREEN_WIDTH, nav_y},
            .color = {128,128,128,255},
            .tag = RECT_TAG
        },
        {   .text_or_uri = "",
            .src_rect = {0, nav_y, SCREEN_WIDTH-180, playground_y},
            .dst_rect = {0, nav_y, SCREEN_WIDTH-180, playground_y},
            .color = {64,64,64,255},
            .tag = RECT_TAG
        },
        {   .text_or_uri = "File",
            .src_rect = {25, 5, 0, nav_y},
            .dst_rect = {25, 5, 0, nav_y},
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
    scene_t* current_scene = _common->GetCurrentScene();
    LOG_INFO("Mouse X: %i, Mouse Y: %i", mouse_x, mouse_y);
    for (uint8_t i = 0; i < current_scene->texture_src_rects.size(); ++i) {
        uint8_t tag = current_scene->tags[i];
        const uint16_t x_offset = SCREEN_WIDTH - 160;
        const uint16_t y_offset = 64;
        if (_common->isBackgroundSpriteTexture(tag)) {
            for (uint8_t i = 0; i < 4; ++i) {
                for (uint8_t j = 0; j < 8; ++j) {
                    const uint16_t sprite_x = x_offset + (i*16*2);
                    const uint16_t sprite_y = y_offset + (j*16*2);
                    if (mouse_x > sprite_x && mouse_x <= sprite_x + 16*2 && mouse_y > sprite_y && mouse_y <= sprite_y + 16*2) {
                        _sprite_selection.selection = true;
                        _sprite_selection.x = i;
                        _sprite_selection.y = j;
                        LOG_INFO("Selection made on sprite.");
                    }
                }
            }
        }
    }
}

void MapEditor::TryToPlace(const int mouse_x, const int mouse_y) {
    if (mouse_x > 0 && mouse_x < SCREEN_WIDTH - 160 && mouse_y > 64 && mouse_y < SCREEN_HEIGHT) {
        _placement.x = mouse_x;
        _placement.y = mouse_y;
        _sprite_selection.selection = false;
    }
}

void MapEditor::UpdateMouseCoords(int x, int y) {
    _mouse_x = x;
    _mouse_y = y;
}

int main() {
    std::shared_ptr<Common> common_ptr = std::make_shared<Common>("Map Editor");
    std::unique_ptr<MapEditor> map_editor = std::make_unique<MapEditor>(common_ptr);

    int quit_app = 0;
    int dt, start_ticks, end_ticks = 0;


    while (quit_app == 0){
        SDL_Event EventHandler;
        while (SDL_PollEvent(&EventHandler) != 0) {
            if (EventHandler.type == SDL_QUIT)
                quit_app = 1;
            else if (EventHandler.type == SDL_MOUSEMOTION) {
                map_editor->UpdateMouseCoords(EventHandler.motion.x, EventHandler.motion.y);
            }
            else if (EventHandler.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState( &x, &y );
                map_editor->UpdateMouseCoords(x, y);
                if (!map_editor->isSelectionActive()) {
                    map_editor->HandleSelection(x, y);
                } else {
                    map_editor->TryToPlace(x, y);
                }
            }
        }

        start_ticks = SDL_GetTicks();
        dt = start_ticks - end_ticks;
        if (dt > 1000 / 60.0) {
            map_editor->RenderCurrentScene();
            end_ticks = start_ticks;
        }
    }
    return 0;
}
