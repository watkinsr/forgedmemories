#include "Common.h"
#include "MapEditor.h"

const int RIGHT_PANEL_WIDTH = 180;
const float MESSAGE_RIGHT_PANEL_WIDTH = 0.90f;

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
            .tag = RECT_TAG,
            .idx = 0
        },
        // Menu item "ADD"
        {   .text_or_uri = "ADD",
            .src_rect = {menu_item_offset + menu_item_width*0, 5, 0, menu_y},
            .dst_rect = {menu_item_offset + menu_item_width*0, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG,
            .idx = 1
        },
        // Menu item "DEL"
        {   .text_or_uri = "DEL",
            .src_rect = {menu_item_offset + menu_item_width*1, 5, 0, menu_y},
            .dst_rect = {menu_item_offset + menu_item_width*1, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG,
            .idx = 2
        },
        // Menu item "SAVE"
        {   .text_or_uri = "SAVE",
            .src_rect = {menu_item_offset + menu_item_width*2, 5, 0, menu_y},
            .dst_rect = {menu_item_offset + menu_item_width*2, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG,
            .idx = 3
        },
        // Menu item "Mark" - Delineate a given map tile by selecting top left position.
        {   .text_or_uri = "MARK",
            .src_rect = {menu_item_offset + menu_item_width*3, 5, 0, menu_y},
            .dst_rect = {menu_item_offset + menu_item_width*3, 5, 0, menu_y},
            .color = {255,255,255,255},
            .font_size = FONT_SIZE::MEDIUM,
            .tag = TEXT_TAG,
            .idx = 4
        },
        // Placement bar rectangle.
        {   .text_or_uri = "",
            .src_rect = {0, menu_y, SCREEN_WIDTH-RIGHT_PANEL_WIDTH, menu_y + placement_bar_y},
            .dst_rect = {0, menu_y, SCREEN_WIDTH-RIGHT_PANEL_WIDTH, menu_y + placement_bar_y},
            .color = {96,96,96,255},
            .tag = RECT_TAG,
            .idx = 5
        },
        // Placement Menu Bar item "File"
        {   .text_or_uri = "Placement area",
            .src_rect = {menu_item_offset, 37, 0, menu_y + placement_bar_y},
            .dst_rect = {menu_item_offset, 37, 0, menu_y + placement_bar_y},
            .color = {255,255,255,255},
            .font_size = FONT_SIZE::SMALL,
            .tag = TEXT_TAG,
            .idx = 6
        },
        // Placement rectangle.
        {   .text_or_uri = "",
            .src_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
            .dst_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
            .color = {64,64,64,255},
            .tag = RECT_TAG,
            .idx = 7
        },
        // Spritesheet.
        {   .text_or_uri = "assets/bg/Berry Garden.png",
            .src_rect = {0, 0, 16, 16},
            .dst_rect = {0, 0, 64, 128},
            .color = {0,0,0,0},
            .tag = SPRITE_TAG | BACKGROUND_SPRITE_FLAG,
            .idx = 8
        },
    };
    _common->AddScene(SCENE_1);
}

void MapEditor::RenderCurrentScene() {
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

    // Cleanup our old dynamic text textures.
    int cleanup_counter = 0;
    while(current_scene->textures.size() > _common->GetInitialSceneTextureSize()) {
        SDL_DestroyTexture(current_scene->textures[current_scene->textures.size() - 1]);
        current_scene->textures.pop_back();
        current_scene->texture_src_rects.pop_back();
        current_scene->texture_dst_rects.pop_back();
        current_scene->tags.pop_back();
        cleanup_counter++;
    }
    // if (cleanup_counter > 0) LOG(1, "INFO", "Cleaned up %i text textures\n", cleanup_counter);
    _messages_flushed = 0;

    if (message.flushable) {
        float y_offset = (SCREEN_HEIGHT * 0.8) + (SCREEN_HEIGHT * 0.05f);
        assert(message.lines.size() > 0);
        int message_offset_x = SCREEN_WIDTH - RIGHT_PANEL_WIDTH + (RIGHT_PANEL_WIDTH-message.line_width);
        for (const auto& line : message.lines) {
            gametexture_t message_texture = {
                .text_or_uri = line,
                .src_rect = {0, 0, 0, 0},
                .dst_rect = {message_offset_x, y_offset, 0, 0},
                .color = {255,255,255,255},
                .font_size = FONT_SIZE::SMALL,
                .tag = TEXT_TAG,
                .idx = 255 // Arbitrary, helps debugging.
            };
            _common->LoadTexture(_scene_stack_idx, std::move(message_texture));
            _messages_flushed++;
            y_offset += 16 + 10;  // Push it down by FONT_SIZE::SMALL + padding.
        }
    }

    float tick = SDL_GetTicks();
    float deltaTick = tick - _prev_tick;
    uint32_t fps = 1000.0f / deltaTick;
    gametexture_t fps_texture = {
        .text_or_uri = "FPS: " + std::to_string(fps),
        .src_rect = {0, 0, 0, 0},
        .dst_rect = {SCREEN_WIDTH - 190, 5, 0, 0},
        .color = {255,255,255,255},
        .font_size = FONT_SIZE::SMALL,
        .tag = TEXT_TAG
    };
    _common->LoadTexture(_scene_stack_idx, std::move(fps_texture));

    _prev_tick = tick;
}


MapEditor::MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    _SetTextureLocations();
    _common->AllocateScene(false);
}

void MapEditor::HandleSelection(const int mouse_x, const int mouse_y) {
    if (!(
        _editor_mode == editor_mode::ADD ||
        _editor_mode == editor_mode::DEL ||
        _editor_mode == editor_mode::MARK
    )) return;
    scene_t* current_scene = _common->GetCurrentScene();
    LOG_INFO("Mouse: [%i, %i]\n", mouse_x, mouse_y);
    if (mouse_x < SCREEN_WIDTH * 0.8 && _editor_mode == editor_mode::DEL) {
        std::sort(_placements.begin(), _placements.end(), [](const Placement& a, const Placement& b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
        });
        int placement_idx = 0;
        for (const auto& placement : _placements) {
            int px = placement.x;
            int py = placement.y;
            if (mouse_x > px && mouse_x < px + 32 && mouse_y > py && mouse_y < py + 32) {
                _placements.erase(_placements.begin() + placement_idx);
                break;
            }
            placement_idx++;
        }
        return;
    };
    if (_editor_mode == editor_mode::ADD) {
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
                            LOG(1, "INFO", "Selection made on sprite.\n");
                        }
                    }
                }
            }
        }
    }
    if (mouse_x < SCREEN_WIDTH * 0.8 && _editor_mode == editor_mode::MARK) {
        std::sort(_placements.begin(), _placements.end(), [](const Placement& a, const Placement& b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
        });
        for (const auto& placement : _placements) {
            int px = placement.x;
            int py = placement.y;
            if (mouse_x > px && mouse_x < px + 32 && mouse_y > py && mouse_y < py + 32) {
                LOG(1, "INFO", "Selected top-left as: [%i, %i]\n", px, py);
            }
        }
        return;
    }
}

void MapEditor::HandleMenuBarSelection(const int mouse_x, const int mouse_y) {
    LOG(1, "TRACE", "MapEditor::HandleMenuBarSelection(mouse_x=%i, mouse_y=%i)\n", mouse_x, mouse_y);
    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);

    if (mouse_x > menu_item_offset && mouse_x < menu_item_offset + menu_item_width*1) {
        message = {
            .lines = {"ADD mode active!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
        _editor_mode = editor_mode::ADD;
    } else if (mouse_x > menu_item_offset + menu_item_width*1 && mouse_x < menu_item_offset + menu_item_width*2) {
        LOG_INFO("DEL menu item selected");
        _editor_mode = editor_mode::DEL;
        message = {
            .lines = {"DEL mode active!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
    } else if (mouse_x > menu_item_offset + menu_item_width*2 && mouse_x < menu_item_offset + menu_item_width*3) {
        LOG_INFO("SAVE menu item selected");
        if (_placements.size() == 0) return;
        // We could probably write a file to be Map1.h where by we create 4x4 map tiles.
        // We have _placements that represent our current "edited map".

        // Sort by y-coordinate with x-coordinate as second priority.
        std::sort(_placements.begin(), _placements.end(), [](const Placement& a, const Placement& b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
        });
        std::vector<Placement> tile = {};
        uint16_t begin_tile_y = _placements[0].y;  // Represents the first "y" within a given 4x4 tile.
        uint8_t map_idx = 0;

        for (const auto& placement : _placements) {
            // HACK: Essentially we're checking if we've moved downwards a full 4x4 tile.
            if (placement.y >= (begin_tile_y + 128)) {
                if (tile.size() > 0) {
                    save_tile(tile, map_idx);
                    map_idx++;
                    tile.resize(0);
                }
                begin_tile_y = placement.y;
            }
            tile.push_back(placement);
        }
        // HACK: And then we make sure that if we didn't move down 4x4 tile, we save it anyway.
        if (tile.size() > 0) save_tile(tile, map_idx);
    } else if (mouse_x > menu_item_offset + menu_item_width*3 && mouse_x < menu_item_offset + menu_item_width*4) {
        LOG(1, "INFO", "MARK menu item selected\n");
        _editor_mode = editor_mode::MARK;
        message = {
            .lines = {"MARK mode active !", "Please select top-left corner of 4x4 tile."},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
    }
}

void MapEditor::save_tile(const vector<Placement>& tile, const uint8_t map_idx) {
    LOG_INFO("[TRACE] MapEditor::save_tile(?)");

    std::ofstream out_file;
    out_file.open(MAP_FILE);
    if (!out_file.is_open()) {
        fprintf(stderr, "save_tile(?) - Failed to open tile");
        return;
    }

    out_file << "#define MAP" << std::to_string(map_idx) << "_METADATA " << std::to_string(tile[0].x) << "," << std::to_string(tile[0].y) << endl;

    out_file << "#define MAP" << std::to_string(map_idx) << " {\\" << endl;
    out_file << "    {";

    // Determine the "left-most" sprite x so we can index every given placement correctly.
    uint8_t leftmost_x = 255;
    for (const auto& placement: tile) {
        if ((placement.x / 32) < leftmost_x) leftmost_x = placement.x / 32;
    }

    uint16_t prev_x = tile[0].x;
    uint16_t prev_y = tile[0].y;
    uint8_t idx = 0;
    uint8_t col_cur = 0;
    for (const auto& placement : tile) {
        LOG_INFO("Placement<x: %i, y: %i>", placement.x, placement.y);

        // State representing Column cursor "jump"
        uint8_t norm_cur_x = (placement.x / 32);
        uint8_t norm_prev_x = (prev_x / 32);
        uint8_t x_norm_jump = 0;
        if (norm_cur_x > norm_prev_x) x_norm_jump = norm_cur_x - norm_prev_x;
        else x_norm_jump = norm_cur_x - leftmost_x + 1; // Special case for first sprite.

        uint8_t x_offset = norm_cur_x - leftmost_x;

        // If we went down a row.
        if (placement.y > prev_y) {
            x_norm_jump = 0;
            // Pad rest of row.
            if (col_cur < 4) {
                LOG_INFO("Pad rest of row.");
                while(col_cur < 4) {
                    if (col_cur != 3) out_file << "-1,";
                    else out_file << "-1";
                    col_cur++;
                }
            }
            out_file << "},\\" << endl;

            col_cur = 0;

            // How many rows did we go down?
            uint8_t norm_y_delta = (placement.y / 32) - (prev_y / 32);

            // Pad remaining rows.
            if (norm_y_delta > 1) {
                LOG_INFO("Pad complete rows.");
                while(norm_y_delta > 1) {
                    out_file << "    {-1,-1,-1,-1},\\" << endl;
                    norm_y_delta--;
                }
            }

            out_file << "    {";

            // Pad beginning of row of placement.
            if (x_offset > 0) LOG_INFO("Pad beginning of row.");
            while(x_offset > 0) {
                out_file << "-1,";
                x_offset--;
                col_cur++;
            }
        }


        // If the column cursor jumped, fill the gap.
        if (x_norm_jump > 1) {
            LOG_INFO("X cursor jumped, fill gap. (%i - %i)", norm_cur_x, norm_prev_x);
            while(x_norm_jump > 1) {
                out_file << "-1,";
                x_norm_jump--;
                col_cur++;
            }
        }
        std::string sprite_encoding = std::to_string((placement.sprite_x_idx * 16) + placement.sprite_y_idx);
        out_file << sprite_encoding;
        if ((idx < tile.size() - 1 && tile[idx+1].y == tile[idx].y) || col_cur < 3) out_file << ",";
        col_cur++;
        idx++;
        prev_x = placement.x;
        prev_y = placement.y;
    }
    // Pad rest of row.
    if (col_cur < 4) LOG_INFO("Pad rest of row.");
    while(col_cur < 4) {
        if (col_cur != 3) out_file << "-1,";
        else out_file << "-1";
        col_cur++;
    }
    col_cur = 0;

    // How many rows remaining?
    uint8_t first_norm_y = (tile[0].y / 32);
    uint8_t last_norm_y = (tile[tile.size() - 1].y / 32);
    uint8_t tile_y_delta = 3 - (last_norm_y - first_norm_y);

    LOG_INFO("Need to pad finally %i rows", tile_y_delta);

    // Finished
    if (tile_y_delta == 0) out_file << "}\\" << endl;

    // Pad remaining rows.
    else {
        if (tile_y_delta > 0) LOG_INFO("Pad complete rows.");
        out_file << "},\\" << endl;

        while(tile_y_delta > 0) {
            if (tile_y_delta != 1) out_file << "    {-1,-1,-1,-1},\\" << endl;
            else out_file << "    {-1,-1,-1,-1}\\" << endl;
            tile_y_delta--;
        }
    }

    out_file << "}" << endl;

    out_file.close();

    if (message.flushable) {
        message = {
            .lines = {"Save successful!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
        };
    }
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

uint64_t frame_count = 0;
uint64_t current_ticks = 0;

void MapEditor::TryLoadPreviousMap() {
    LOG(1, "TRACE", "TryLoadPreviousMap()\n");
    std::ifstream file(MAP_FILE);
    if (!file.is_open()) {
        LOG(1, "PANIC", "Failed to open previous map at file: %s\n", MAP_FILE);
        exit(EXIT_FAILURE);
    }

    std::string line;
    int x, y;
    std::vector<std::vector<int>> tiles = {{}};
    int tile_idx = 0;
    while (std::getline(file, line)) {
        if (line.starts_with("#define MAP0_METADATA")) {
            line = line.substr(sizeof("#define MAP0_METADATA"));
            size_t pos;
            while ((pos = line.find(",")) != std::string::npos) {
                x = atoi(line.substr(0, pos).c_str());
                line.erase(0, pos + 1);
                break;
            }
            y = atoi(line.c_str());
            std::cout << "X: " << x << ", Y: " << y << std::endl;
        } else if (line.starts_with("#define MAP")) continue;
        else {
            size_t pos;
            if (pos = line.find("{") == std::string::npos) continue;
            line.erase(std::remove(line.begin(), line.end(), '{'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '}'), line.end());

            while ((pos = line.find(",")) != std::string::npos) {
                if (tiles[tile_idx].size() == 0) tiles[tile_idx] = std::vector<int>();
                tiles[tile_idx].push_back(atoi(line.substr(0, pos).c_str()));
                line.erase(0, pos + 1);
            }
            if (tile_idx == 3) tiles[tile_idx].push_back(atoi(line.c_str()));
            tile_idx++;
            if (tile_idx == 4) break;
            std::vector<int> v = {};
            tiles.push_back(std::move(v));
        }
    }


    for (const auto& tile : tiles) {
        std::cout << "[";
        for (const auto& v : tile) {
            std::cout << v << ", ";
        }
        std::cout << "]";
        std::cout << std::endl;
    }
    file.close();

    _prev_map.first_tile_x = x;
    _prev_map.first_tile_y = y;
    _prev_map.tiles = std::move(tiles);

    Placement placement = {
        .x = _prev_map.first_tile_x,
        .y = _prev_map.first_tile_y,
    };

    int x_offset_idx = 0;
    int y_offset_idx = 0;
    uint8_t x_idx = 0;
    uint8_t y_idx = 0;
    bool first_placement_set = false;

    for (const auto& tile : _prev_map.tiles) {
        for (const auto& v : tile) {
            if (v != -1 && !first_placement_set) {
                placement.sprite_x_idx = v / 16;
                placement.sprite_y_idx = v % 16;
                x_offset_idx = x_idx;
                _placements.push_back(placement);
                first_placement_set = true;
            } else if (v != -1) {
                const int x_offset = x_idx - x_offset_idx;
                const int y_offset = y_idx - y_offset_idx;
                Placement p = Placement {
                    .x = _prev_map.first_tile_x + (x_offset * 32),
                    .y = _prev_map.first_tile_y + (y_offset * 32),
                    .sprite_x_idx = v / 16,
                    .sprite_y_idx = v % 16,
                };
                _placements.push_back(std::move(p));
            }
            x_idx++;
        }
        y_idx++;
        x_idx = 0;
    }
    LOG_INFO("Placements after loading previous map: %u\n", _placements.size());
}

static std::shared_ptr<Common> common_ptr;
static std::unique_ptr<MapEditor> map_editor;

const int BACKBUFFER_WIDTH = 800;
const int BACKBUFFER_HEIGHT = 600;

void initialize_the_mapeditor() {
    LOG(1, "TRACE", "initialize_the_mapeditor()\n");
    SetMapFile();
    std::string app_name = std::string("MapEditor");
    common_ptr = std::make_shared<Common>(std::move(app_name), BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
    LOG_INFO("Able to construct Common\n");
    map_editor = std::make_unique<MapEditor>(common_ptr);
    LOG_INFO("Able to construct Game\n");

    map_editor->message = {
        .lines = {"No mode currently active."},
        .word_wrap = true,
        .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
    };

    map_editor->TryLoadPreviousMap();
    // FIXME: map_editor->FillBackBufferInitial();
}

static Uint64 tick = SDL_GetTicks64();     // SDL Library is initialized above via Common::Common()
static Uint64 last_frame_tick = tick - 17; // Ensures we always render the first frame straight-away.
static Uint64 dt_frame = 17;               // Ensures no initial event wait.
static Uint64 frame_per_second = 0;        // Counter that gets reset after 1s.
static Uint64 seconds = 1;                 // Seconds rendered.

static void mainloop(void) {
    int quit_app = 0;

    SDL_Event eh;

    int DEFAULT_WAIT = 13;
    int timeout = 0;

    bool interactive = true;

    if (dt_frame < DEFAULT_WAIT && timeout != 0) timeout = DEFAULT_WAIT - dt_frame; // Allow 3ms to draw or handle interaction.

    while (SDL_WaitEventTimeout(&eh, timeout) != 0) {
        if (eh.type == SDL_QUIT) {
            // Cleanup all current scene textures.
            auto current_scene = common_ptr->GetCurrentScene();
            uint8_t textures_to_clean = current_scene->textures.size();
            uint8_t i = textures_to_clean;
            while(i > 0) {
                SDL_DestroyTexture(current_scene->textures[common_ptr->GetInitialSceneTextureSize() - 1 + i]);
                current_scene->textures.pop_back();
                current_scene->texture_src_rects.pop_back();
                current_scene->texture_dst_rects.pop_back();
                current_scene->tags.pop_back();
                i--;
            }

            TTF_Quit();
            SDL_Quit();
            exit(1);
        }
        else if (eh.type == SDL_MOUSEMOTION) {
            map_editor->UpdateMouseCoords(eh.motion.x, eh.motion.y);
        }
        else if (eh.type == SDL_MOUSEBUTTONDOWN) {
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

    tick = SDL_GetTicks64();
    dt_frame = tick - last_frame_tick;
    if (dt_frame > DEFAULT_WAIT && interactive) { // Allow 2ms draw time.
        Uint64 before = SDL_GetTicks64();
        map_editor->RenderCurrentScene();

        // Steady clock, eval did render a frame in <= 16ms? -> 60FPS
        // If it's > 16ms, eval (x / 1000ms) * 60frames
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - map_editor->prev_clock).count();
        map_editor->prev_clock = now;
        // LOG(1, "INFO", "ms elapsed between tick: %ld\n", ms);
        if (ms <= 16) {
            map_editor->set_fps(60);
        } else {
            {
                uint8_t fps = ((float)(ms)/1000.0f)*60;
                map_editor->set_fps(fps);
            }
        }
        Uint64 after = SDL_GetTicks64();
        interactive = false;
        last_frame_tick = tick;
        dt_frame = SDL_GetTicks64() - after;
    }
}

int main() {
    initialize_the_mapeditor();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 60, 1);
    #else
    while (1) { mainloop(); }
    #endif
    return 0;
}


// TODO: Introduce grouping functionality that allows picking top-left/bottom-right of a given group.
// TODO: Introduce "player" sprite that can designate player start location.
// TODO: Introduce letf/up/right/down keys to move the snapping grid.
