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
        SDL_Rect src_rect = current_scene->texture_src_rects[i];
        SDL_Rect dst_rect = current_scene->texture_dst_rects[i];
        uint8_t tag = current_scene->tags[i];
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

            // Render drag&drop
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

            // Render placements.
            for (auto placement : _placements) {
                src_rect.x = placement.sprite_x_idx*16;
                src_rect.y = placement.sprite_y_idx*16;
                src_rect.w = 16;
                src_rect.h = 16;
                dst_rect.x = placement.x;
                dst_rect.y = placement.y;
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

    // Render the "snap" lines in the placement area.
    SDL_SetRenderDrawColor(_renderer, 0x00, 0xFF, 0x00, 0xFF);

    // Note: Offset=68 since menu_bar_y_offset=57, sprite_2x_scale_y=32"
    uint16_t line_y_offset = 64;
    uint16_t line_x_offset = 0;

    const int screen_y_mod = SCREEN_HEIGHT % 32;

    // Draw's the horizontal lines.
    while(line_y_offset < SCREEN_HEIGHT + (screen_y_mod)) {
        SDL_RenderDrawLine(_renderer, 0, line_y_offset, SCREEN_WIDTH-192, line_y_offset);
        line_y_offset+=32;
    }

    // Draw's the vertical lines.
    while(line_x_offset < SCREEN_WIDTH-180) {
        SDL_RenderDrawLine(_renderer, line_x_offset, 64, line_x_offset, line_y_offset - 32);
        line_x_offset+=32;
    }

    SDL_RenderPresent(_renderer);

    if (current_scene->textures.size() == _common->GetInitialSceneTextureSize() + 1) {
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
        .dst_rect = {SCREEN_WIDTH - 190, 5, 0, 0},
        .color = {255,255,255,255},
        .font_size = Common::FONT_SIZE::SMALL,
        .tag = TEXT_TAG
    };

    _prev_tick = tick;

    _common->LoadTexture(_scene_stack_idx, std::move(fps_texture));
}

void MapEditor::_SetTextureLocations() {
    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);

    float placement_bar_y_factor = 0.03f;
    int placement_bar_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(placement_bar_y_factor);

    int playground_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(1.0f - menu_y_factor - placement_bar_y_factor);

    const vector<gametexture_t> SCENE_1 = {
        // Menu bar rectangle.
        {   .text_or_uri = "",
            .src_rect = {0, 0, SCREEN_WIDTH, menu_y},
            .dst_rect = {0, 0, SCREEN_WIDTH, menu_y},
            .color = {128,128,128,255},
            .tag = RECT_TAG
        },
        // Menu item "File"
        {   .text_or_uri = "File",
            .src_rect = {25, 5, 0, menu_y},
            .dst_rect = {25, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG
        },
        // Menu item "DEL"
        {   .text_or_uri = "DEL",
            .src_rect = {245, 5, 0, menu_y},
            .dst_rect = {245, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG
        },
        // Menu item "SAVE"
        {   .text_or_uri = "SAVE",
            .src_rect = {300, 5, 0, menu_y},
            .dst_rect = {300, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG
        },
        // Menu item "Mode: "
        {   .text_or_uri = "Mode: ",
            .src_rect = {SCREEN_WIDTH - 250, 5, 0, menu_y},
            .dst_rect = {SCREEN_WIDTH - 250, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::SMALL,
            .tag = TEXT_TAG
        },
        // Placement bar rectangle.
        {   .text_or_uri = "",
            .src_rect = {0, menu_y, SCREEN_WIDTH-180, menu_y + placement_bar_y},
            .dst_rect = {0, menu_y, SCREEN_WIDTH-180, menu_y + placement_bar_y},
            .color = {96,96,96,255},
            .tag = RECT_TAG
        },
        // Placement Menu Bar item "File"
        {   .text_or_uri = "Placement area",
            .src_rect = {25, 37, 0, menu_y + placement_bar_y},
            .dst_rect = {25, 37, 0, menu_y + placement_bar_y},
            .color = {255,255,255,255},
            .font_size = Common::FONT_SIZE::SMALL,
            .tag = TEXT_TAG
        },
        // Placement rectangle.
        {   .text_or_uri = "",
            .src_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
            .dst_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
            .color = {64,64,64,255},
            .tag = RECT_TAG
        },
        // Spritesheet.
        {   .text_or_uri = "assets/bg/Berry Garden.png",
            .src_rect = {0, 0, 16, 16},
            .dst_rect = {0, 0, 64, 128},
            .color = {0,0,0,0},
            .tag = SPRITE_TAG | BACKGROUND_SPRITE_FLAG
        },
    };
    _common->SetInitialSceneTextureSize(SCENE_1.size());
    _common->AddScene(SCENE_1);
}

MapEditor::MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    _SetTextureLocations();
    _common->AllocateScene(false);
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

void MapEditor::HandleMenuBarSelection(const int mouse_x, const int mouse_y) {
    LOG_INFO("MapEditor::HandleMenuBarSelection(mouse_x=%i, mouse_y=%i)", mouse_x, mouse_y);
    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);
    if (mouse_x < 245 || mouse_y > menu_y || mouse_x > 350) return;
    // DEL selection
    if (mouse_x < 300) {
        LOG_INFO("DEL menu item selected");
    } else {
        LOG_INFO("SAVE menu item selected");
        if (_placements.size() == 0) return;
        // We could probably write a file to be Map1.h where by we create 4x4 map tiles.
        // We have _placements that represent our current "edited map".

        // First we need to sort by y-coordinate with x as second priority.
        std::sort(_placements.begin(), _placements.end(), [](const Placement& a, const Placement& b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
        });
        std::vector<Placement> tile = {};
        uint16_t begin_tile_y = _placements[0].y;  // Represents the first "y" within a given 4x4 tile.

        for (const auto& placement : _placements) {
            if (placement.y >= (begin_tile_y + 128)) {
                // LOG_INFO("End of 4x4: %i", (begin_tile_y + 128));
                if (tile.size() > 0) {
                    save_tile(tile);
                    tile.resize(0);
                }
                begin_tile_y = placement.y;
            }
            tile.push_back(placement);
        }
        if (tile.size() > 0) save_tile(tile);
    }
}

void MapEditor::save_tile(const vector<Placement>& tile) {
    LOG_INFO("MapEditor::save_tile(?)");
    std::ofstream out_file;
    out_file.open("/home/ryanwatkins/workplace/rom/include/Map1.h");
    if (!out_file.is_open()) {
        fprintf(stderr, "save_tile(?) - Failed to open tile");
        return;
    }

    out_file << "#define MAP {\\" << endl;
    out_file << "{";
    uint16_t y = tile[0].y;
    uint8_t idx = 0;
    for (const auto& placement : tile) {
        LOG_INFO("Placement<x: %i, y: %i>", placement.x, placement.y);
        if (placement.y > y) {
            out_file << "},\\" << endl << "{";
            y = placement.y;
        };
        out_file << std::to_string((placement.sprite_x_idx * 16) + placement.sprite_y_idx);
        if (idx < tile.size() - 1 && tile[idx+1].y == y) out_file << ",";
        idx++;
    }
    out_file << "}\\" << endl << "}" << endl;

    out_file.close();
}

void MapEditor::TryToPlace(const int mouse_x, const int mouse_y) {
    LOG_INFO("MapEditor::TryToPlace(mouse_x=%i, mouse_y=%i)", mouse_x, mouse_y);

    // Note: Selection cursor is top left of the sprite plus some padding.
    // Therefore it's best to take a "good guess" on what the sprite x/y is.

    const int sprite_x = mouse_x + 16; // Simulate mouse x as being centre x of the sprite.
    const int sprite_y = mouse_y + 16; // Simulate mouse y as being centre y of the sprite

    LOG_INFO("MapEditor::TryToPlace(?) - sprite_x=%i, sprite_y=%i", sprite_x, sprite_y);

    // Check if it can be placed in the Placement Area.
    if (sprite_x >= SCREEN_WIDTH - 160 || sprite_y < 64) return;

    // Snap grid starts at 64. No need to mod this value for now.

    Placement placement;

    // Adjust origin back to top-left of the sprite.
    placement.x = sprite_x - (sprite_x % 32);
    placement.y = sprite_y - (sprite_y % 32);

    placement.sprite_x_idx = _sprite_selection.x;
    placement.sprite_y_idx = _sprite_selection.y;
    _placements.push_back(placement);
    _sprite_selection.selection = false;
}

void MapEditor::UpdateMouseCoords(int x, int y) {
    // LOG_INFO("MapEditor::UpdateMouseCoords(x=%i, y=%i)", x, y);
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
                    if (!map_editor->isSelectionActive()) {
                        map_editor->HandleMenuBarSelection(x, y);
                    }
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
// TODO: Placements need to be removable (Deletion mode).
// TODO: Placements need to be saved (SAVE menu Implementation).
