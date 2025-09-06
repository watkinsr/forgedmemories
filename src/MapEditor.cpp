#define SPRITE_SELECTION_PANE_WIDTH 160
#define SPRITE_TABLE_SELECTION_PANE_Y_OFFSET 64
#define SPRITE_TABLE_COLS 4
#define SPRITE_TABLE_ROWS 8

// C++ Semantics are kind of annoying regarding stack and heap.
// Granular control of memory is a good thing to achieve.
#define ARENA_IMPLEMENTATION
#include "arena.h"

static Arena default_arena = {0};
static Arena temporary_arena = {0};

#include <cmath>

#include "Common.h"
#include "Macros.h"
#include "MapEditor.h"

#define PLACEMENT_MOVE_GRID_UP 1
#define PLACEMENT_MOVE_GRID_DOWN 2
#define PLACEMENT_MOVE_GRID_LEFT 4
#define PLACEMENT_MOVE_GRID_RIGHT 8

#define PLACEMENT_TILE_DIST 32

constexpr uint8_t bg_sprite_tag     = static_cast<uint8_t>(SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
constexpr uint8_t player_sprite_tag = static_cast<uint8_t>(SPRITE_TAG | PLAYER_SPRITE_FLAG);

typedef struct Camera {
    int64_t x;
    int64_t y;
};

Camera camera = {0};

Marked_Maps *marked_maps = NULL;

static std::shared_ptr<Common> common_ptr;
static std::unique_ptr<MapEditor> map_editor;

inline std::ofstream open_map_file() {
    LOG(1, "TRACE", "open_map_file()\n");
    std::ofstream out_file;
    out_file.open(MAP_FILE);
    if (!out_file.is_open()) {
        fprintf(stderr, "save_tile(?) - Failed to open tile");
        ASSERT_NOT_REACHED();
    }
    return out_file;
}

inline void finish_save_map(Message* message, std::ofstream& out_file) {
    LOG(1, "TRACE", "finish_save_map(message=%p, out_file=?)\n", message);
    out_file.close();

    if (message->flushable) {
        *message = {
            .lines = {"Save successful!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
        };
    }    
}

inline SDL_Texture* get_texture_by_entity_tag(SceneData* scene_data, uint8_t tag) {
    SDL_Texture* data;
    auto [it, end] = scene_data->entities.equal_range(tag);
    for (; it != end; ++it) {
        const Entity entity = it->second;
        data = entity.data;
        LOG(1, "VERBOSE", "BlitNewPlacement - (Tag=%u, Ptr=%p)\n", tag, (void*)data);
    }
    return data;
}

void inline BlitNewPlacement(const Placement& placement) {
    LOG(1, "TRACE", "MapEditor::BlitNewPlacement\n");
    SceneData* scene_data = common_ptr->GetSceneData();

    SDL_Rect src;
    SDL_Rect dst;

    src = {placement.sprite_x_idx*16, placement.sprite_y_idx*16, 16, 16};
    dst = {placement.x, placement.y, 32, 32};

    SDL_Renderer* _renderer = common_ptr->GetRenderer();
    SDL_SetRenderTarget(_renderer, common_ptr->GetBackBuffer()); // Blit to the back buffer.

    SDL_Texture* data = get_texture_by_entity_tag(scene_data, placement.tag);
    SDL_RenderCopy(_renderer, data, &src, &dst);
}



static inline void BlitDragAndDrop(const SpriteSelection& entity, int mouse_x, int mouse_y) {
    LOG(1, "VERBOSE", "MapEditor::BlitDragAndDrop()\n");
    SDL_Renderer* _renderer = common_ptr->GetRenderer();

    SDL_Rect src;
    SDL_Rect dst;

    src.x = entity.x*16;
    src.y = entity.y*16;
    src.w = 16;
    src.h = 16;

    dst.x = mouse_x;
    dst.y = mouse_y;
    dst.w = 32;
    dst.h = 32;

    SceneData* scene_data = common_ptr->GetSceneData();
    SDL_Texture* data = get_texture_by_entity_tag(scene_data, entity.tag);
    LOG(1, "VERBOSE", "BlitDragAndDrop - (Tag=%u, Ptr=%p)\n", entity.tag, (void*)data);
    SDL_RenderCopy(_renderer, data, &src, &dst);
}

// Forward Declarations
void delineate_new_map_border(int top_left_x, int top_right_y);
ssize_t binary_search_region(std::set<Placement, OrderHorizontally>*, Vector2D*, Vector2D*, int, int);
void save_map(const vector<Placement>& tile, const uint8_t map_idx, const Vector2D* marked_map, std::ofstream& out_file);
void handle_save_selection(Message*);
void HandleAddDragAndDrop(scene_t*, SpriteSelection*, std::shared_ptr<Common>, const int, const int);

void inline BlitMarkedMap(int top_left_x, int top_right_y) {
    LOG(1, "TRACE", "MapEditor::BlitMarkedMap\n");

    SDL_Renderer* _renderer = common_ptr->GetRenderer();
    SDL_SetRenderTarget(_renderer, common_ptr->GetBackBuffer()); // Blit to the back buffer.

    SDL_SetRenderDrawColor(_renderer, 0xFF, 0x00, 0x00, 0xFF);

    // int SDL_RenderDrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2);
    SDL_RenderDrawLine(_renderer, top_left_x, top_right_y, top_left_x, top_right_y+128);         // Top line
    SDL_RenderDrawLine(_renderer, top_left_x, top_right_y, top_left_x+128, top_right_y);         // Left line
    SDL_RenderDrawLine(_renderer, top_left_x+128, top_right_y, top_left_x+128, top_right_y+128); // Right line
    SDL_RenderDrawLine(_renderer, top_left_x, top_right_y+128, top_left_x+128, top_right_y+128); // Bottom line
}

void store_placement(std::shared_ptr<Common> common_ptr, Placement p) {
    auto [it, inserted] = g_placements.insert(std::move(p));
    if (inserted) g_placements_by_x_ptrs.insert(&*it);
    LOG(1, "INFO", "Stored <Placement [%i,%i]>\n", p.x, p.y);
}

SceneData GetFirstScene() {
    LOG(1, "TRACE", "MapEditor::GetFirstScene()\n");
    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);

    float placement_bar_y_factor = 0.03f;
    int placement_bar_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(placement_bar_y_factor);

    int playground_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(1.0f - menu_y_factor - placement_bar_y_factor);

    SDL_Color default_menu_text_color = {255,255,255,255};

    SceneData data = {};
    // Menu bar rectangle.
    data.entities.insert({ RECT_TAG, Entity {
        .src_rect = {0, 0, SCREEN_WIDTH, menu_y},
        .dst_rect = {0, 0, SCREEN_WIDTH, menu_y},
        .color = {128,128,128,255},
        .tag = RECT_TAG,
        .data = nullptr
    }});

    // Placement bar rectangle.
    data.entities.insert({RECT_TAG, Entity {
        .src_rect = {0, menu_y, SCREEN_WIDTH-RIGHT_PANEL_WIDTH, menu_y + placement_bar_y},
        .dst_rect = {0, menu_y, SCREEN_WIDTH-RIGHT_PANEL_WIDTH, menu_y + placement_bar_y},
        .color = {96,96,96,255},
        .tag = RECT_TAG,
        .data = nullptr
    }});

    // Placement rectangle.
    data.entities.insert({RECT_TAG, Entity {
        .src_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
        .dst_rect = {0, menu_y + placement_bar_y, SCREEN_WIDTH-180, playground_y},
        .color = {64,64,64,255},
        .tag = RECT_TAG,
        .data = nullptr
    }});

    // Add menu item
    SDL_Rect add_dst_rect = {menu_item_offset + menu_item_width*0, 5, 0, menu_y};
    SDL_Texture* add_data = common_ptr->LoadText(FONT_SIZE::MEDIUM, "ADD", &add_dst_rect, default_menu_text_color);
    data.entities.insert({TEXT_TAG, Entity {
        .src_rect = {menu_item_offset + menu_item_width*0, 5, 0, menu_y},
        .dst_rect = add_dst_rect,
        .color = default_menu_text_color,
        .tag = TEXT_TAG,
        .data = add_data
    }});

    // Del menu item
    SDL_Rect del_dst_rect = {menu_item_offset + menu_item_width*1, 5, 0, menu_y};
    SDL_Texture* del_data = common_ptr->LoadText(FONT_SIZE::MEDIUM, "DEL", &del_dst_rect, default_menu_text_color);
    data.entities.insert({TEXT_TAG, Entity {
        .src_rect = {menu_item_offset + menu_item_width*1, 5, 0, menu_y},
        .dst_rect = del_dst_rect,
        .color = default_menu_text_color,
        .tag = TEXT_TAG,
        .data = del_data
    }});

    // Save menu item
    SDL_Rect save_dst_rect = {menu_item_offset + menu_item_width*2, 5, 0, menu_y};
    SDL_Texture* save_data = common_ptr->LoadText(FONT_SIZE::MEDIUM, "SAVE", &save_dst_rect, default_menu_text_color);
    data.entities.insert({TEXT_TAG, Entity {
        .src_rect = {menu_item_offset + menu_item_width*2, 5, 0, menu_y},
        .dst_rect = save_dst_rect,
        .color = default_menu_text_color,
        .tag = TEXT_TAG,
        .data = save_data
    }});

    // Mark menu item
    SDL_Rect mark_dst_rect = {menu_item_offset + menu_item_width*3, 5, 0, menu_y};
    SDL_Texture* mark_data = common_ptr->LoadText(FONT_SIZE::MEDIUM, "MARK", &mark_dst_rect, default_menu_text_color);
    data.entities.insert({TEXT_TAG, Entity {
        .src_rect = {menu_item_offset + menu_item_width*3, 5, 0, menu_y},
        .dst_rect = mark_dst_rect,
        .color = default_menu_text_color,
        .tag = TEXT_TAG,
        .data = mark_data
    }});

    // File menu item
    SDL_Rect file_dst_rect = {menu_item_offset, 37, 0, menu_y + placement_bar_y};
    SDL_Texture* file_data = common_ptr->LoadText(FONT_SIZE::SMALL, "Placement area", &file_dst_rect, default_menu_text_color);
    data.entities.insert({TEXT_TAG, Entity {
        .src_rect = {menu_item_offset, 37, 0, menu_y + placement_bar_y},
        .dst_rect = file_dst_rect,
        .color = default_menu_text_color,
        .tag = TEXT_TAG,
        .data = file_data
    }});

    // Spritesheet.
    data.entities.insert({bg_sprite_tag, Entity {
        .src_rect = {0, 0, 16, 16},
        .dst_rect = {0, 0, 64, 128},
        .color = {0,0,0,0},
        .tag = bg_sprite_tag,
        .data = common_ptr->LoadImage("assets/bg/Berry Garden.png")
    }});

    // Player sprite.
    data.entities.insert({player_sprite_tag, Entity {
        .src_rect = {PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT},
        .dst_rect = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT},
        .color = {0,0,0,0},
        .tag = player_sprite_tag,
        .data = common_ptr->LoadImage("assets/player2.png")
    }});

    return data;
}

void draw_horizontal_strip(SDL_Renderer* renderer, uint32_t y_offset_begin, uint32_t y_offset_end) {
    LOG(1, "TRACE", "draw_horizontal_strip(renderer=?)\n");
    const int placement_bar_width = SCREEN_WIDTH-180;
    SDL_Rect fill_rect;

    auto y_offset = y_offset_begin;
    auto x_offset = 0;

    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    fill_rect = {0, y_offset_begin, placement_bar_width, PLACEMENT_TILE_DIST};
    SDL_RenderFillRect(renderer, &fill_rect);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    while(y_offset < y_offset_end) { // Draw's the horizontal lines.
        SDL_RenderDrawLine(renderer, 0, y_offset, SCREEN_WIDTH-192, y_offset);
        y_offset+=32;
    }

    while(x_offset < SCREEN_WIDTH-180) { // Draw's the vertical lines.
        SDL_RenderDrawLine(renderer, x_offset, y_offset_begin, x_offset, y_offset_begin + PLACEMENT_TILE_DIST);
        x_offset+=32;
    }
}

void draw_vertical_strip(SDL_Renderer* renderer, uint32_t x_offset_begin, uint32_t x_offset_end, int grid_height) {
    LOG(1, "TRACE", "draw_vertical_strip(renderer=?, x_offset_begin=%u, x_offset_end=%u, grid_height=%i)\n", x_offset_begin, x_offset_end, grid_height);
    const int grid_begin_y = 64;
    SDL_Rect fill_rect;

    auto y_offset = grid_begin_y;
    auto x_offset = x_offset_begin;

    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    fill_rect = {x_offset_begin, grid_begin_y, PLACEMENT_TILE_DIST, grid_height};
    SDL_RenderFillRect(renderer, &fill_rect);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    while(y_offset < SCREEN_HEIGHT) { // Draw's the horizontal lines.
        SDL_RenderDrawLine(renderer, x_offset_begin, y_offset, x_offset_end, y_offset);
        y_offset+=PLACEMENT_TILE_DIST;
    }

    while(x_offset < SCREEN_WIDTH-180) { // Draw's the vertical lines.
        SDL_RenderDrawLine(renderer, x_offset, grid_begin_y, x_offset, y_offset-PLACEMENT_TILE_DIST);
        x_offset+=PLACEMENT_TILE_DIST;
    }
    return;
}

typedef struct NextPlacements {
    Placement** data;
    size_t len;
};

NextPlacements search_next_placements(uint8_t direction) {
    LOG(1, "TRACE", "search_next_placements(direction=%u) - camera(x=%i, y=%i)\n", direction, camera.x, camera.y);

    NextPlacements next_placements = {0};

    const auto origin_end_placement_area_x = ((SCREEN_WIDTH-212)/32) * 32;
    const auto origin_end_placement_area_y = ((SCREEN_HEIGHT-64)/32) * 32;
    int64_t next_strip_y;
    int64_t next_strip_x;
    Placement lo;
    Placement hi;
    if (direction == PLACEMENT_MOVE_GRID_DOWN) {
        next_strip_y = ((camera.y  + PLACEMENT_TILE_DIST) - 64) * -1;
    } else if (direction == PLACEMENT_MOVE_GRID_UP) {
        next_strip_y = origin_end_placement_area_y + (camera.y - PLACEMENT_TILE_DIST) * -1;
    } else if (direction == PLACEMENT_MOVE_GRID_LEFT) {
        next_strip_x = origin_end_placement_area_x + (camera.x - PLACEMENT_TILE_DIST) * -1;
    } else {
        next_strip_x = (camera.x  + PLACEMENT_TILE_DIST) * -1;
    }

    bool is_horizontal_movement = (direction == PLACEMENT_MOVE_GRID_LEFT || direction == PLACEMENT_MOVE_GRID_RIGHT);
    bool is_vertical_movement = (direction == PLACEMENT_MOVE_GRID_DOWN || direction == PLACEMENT_MOVE_GRID_UP);

    if (is_vertical_movement) {
        lo =  {.x = std::numeric_limits<int16_t>::min(), .y = next_strip_y};
        hi =  {.x = std::numeric_limits<int16_t>::max(), .y = next_strip_y};
    } else {
        lo =  {.x = next_strip_x, .y = std::numeric_limits<int16_t>::min()};
        hi =  {.x = next_strip_x, .y = std::numeric_limits<int16_t>::max()};
    }

    LOG(1, "INFO", "camera(x=%i, y=%i) origin(end=%i)\n", camera.x, camera.y, origin_end_placement_area_x);
    LOG(1, "INFO", "lo(x=%i, y=%i), hi(x=%i, y=%i), origin(begin=?, end=%u)\n", lo.x, lo.y, hi.x, hi.y, origin_end_placement_area_y);

    if (is_horizontal_movement) {
        auto first = g_placements_by_x_ptrs.lower_bound(&lo);
        auto last = g_placements_by_x_ptrs.upper_bound(&hi);

        next_placements.len = std::distance(first, last);
        if (next_placements.len > 0) {
            Placement** ptrs = new Placement*[next_placements.len];
            std::size_t i = 0;
            for (auto it = first; it != last; ++it, ++i) {
                Placement* placement_ptr = *it;
                ptrs[i] = placement_ptr;
            }
            next_placements.data = ptrs;
        }
    }

    if (is_vertical_movement) {
        auto first = g_placements.lower_bound(lo);
        auto last = g_placements.upper_bound(hi);

        next_placements.len = std::distance(first, last);
        if (next_placements.len > 0) {
            Placement** ptrs = new Placement*[next_placements.len];
            std::size_t i = 0;
            for (auto it = first; it != last; ++it, ++i) {
                ptrs[i] = const_cast<Placement*>(&*it);
            }
            next_placements.data = ptrs;
        }
    }
    return next_placements;
}

void inline draw_next_placements(SDL_Renderer* _renderer, NextPlacements& next_placements, SDL_Rect* src, SDL_Rect* dst, SceneData* scene_data) {
    SDL_Texture* data;
    for (auto it = g_placements.begin(); it != g_placements.end(); ++it) {
        const Placement& p = *it;
        LOG(1, "INFO", "<Placement .x=%i, .y=%i>\n", p.x, p.y);
    }
    for (std::size_t i = 0; i < next_placements.len; ++i) {
        const Placement& placement = *(next_placements.data[i]);
        LOG(1, "INFO", "Found <Placement .x=%i, .y=%i>\n", placement.x, placement.y);
        *src = {placement.sprite_x_idx*16, placement.sprite_y_idx*16, 16, 16};
        *dst = {placement.x + camera.x, placement.y + camera.y, 32, 32};
        data = get_texture_by_entity_tag(scene_data, placement.tag);
        SDL_RenderCopy(_renderer, data, src, dst);
    }
}

void MapEditor::BlitPlacementArea(uint8_t direction) {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* t = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        BACKBUFFER_WIDTH,
        BACKBUFFER_HEIGHT
    );

    SDL_Rect src;
    SDL_Rect dst;

    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);

    float placement_bar_y_factor = 0.03f;
    int placement_bar_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(placement_bar_y_factor);

    const int grid_begin_y = 64;

    const int placement_bar_width = SCREEN_WIDTH-180;
    const int grid_height = (SCREEN_HEIGHT - grid_begin_y)-((SCREEN_HEIGHT - grid_begin_y) % PLACEMENT_TILE_DIST);

    SDL_SetRenderTarget(_renderer, t);                              // Switch to our temporary back buffer holder texture.
    SDL_Texture* _back_buffer = _common->GetBackBuffer();
    SceneData* scene_data = common_ptr->GetSceneData();

    uint32_t offset_x_begin;

    NextPlacements next_placements = search_next_placements(direction);

    switch(direction) {
        case PLACEMENT_MOVE_GRID_UP:
            LOG(1, "INFO", "MapEditor::GetNextBackBuffer - Move back buffer up by %u pixels\n", PLACEMENT_TILE_DIST);
            camera.y -= PLACEMENT_TILE_DIST;
            src = {0, grid_begin_y+PLACEMENT_TILE_DIST, placement_bar_width, grid_height-PLACEMENT_TILE_DIST};
            dst = {0, grid_begin_y, placement_bar_width, grid_height-PLACEMENT_TILE_DIST};
            SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy shifted placement grid.
            draw_horizontal_strip(_renderer, grid_begin_y+(grid_height-PLACEMENT_TILE_DIST), SCREEN_HEIGHT);
            draw_next_placements(_renderer, next_placements, &src, &dst, scene_data);
            break;
        case PLACEMENT_MOVE_GRID_DOWN:
            camera.y += PLACEMENT_TILE_DIST;
            LOG(1, "INFO", "MapEditor::GetNextBackBuffer - Move back buffer down by %u pixels\n", PLACEMENT_TILE_DIST);
            src = {0, grid_begin_y, placement_bar_width, grid_height-PLACEMENT_TILE_DIST};
            dst = {0, grid_begin_y+PLACEMENT_TILE_DIST, placement_bar_width, grid_height-PLACEMENT_TILE_DIST};
            SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy shifted placement grid.
            draw_horizontal_strip(_renderer, grid_begin_y, grid_begin_y+PLACEMENT_TILE_DIST);
            draw_next_placements(_renderer, next_placements, &src, &dst, scene_data);
            break;
        case PLACEMENT_MOVE_GRID_LEFT:
            camera.x -= PLACEMENT_TILE_DIST;
            LOG(1, "INFO", "MapEditor::GetNextBackBuffer - Move back buffer left by %u pixels\n", PLACEMENT_TILE_DIST);
            src = {PLACEMENT_TILE_DIST, grid_begin_y, placement_bar_width-PLACEMENT_TILE_DIST, grid_height};
            dst = {0, grid_begin_y, placement_bar_width-PLACEMENT_TILE_DIST, grid_height};
            SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy shifted placement grid.
            offset_x_begin = (SCREEN_WIDTH-180) - ((SCREEN_WIDTH-180) % PLACEMENT_TILE_DIST) - PLACEMENT_TILE_DIST;
            draw_vertical_strip(_renderer, offset_x_begin, offset_x_begin+PLACEMENT_TILE_DIST, grid_height);
            draw_next_placements(_renderer, next_placements, &src, &dst, scene_data);
            break;
        case PLACEMENT_MOVE_GRID_RIGHT:
            camera.x += PLACEMENT_TILE_DIST;
            LOG(1, "INFO", "MapEditor::GetNextBackBuffer - Move back buffer right by %u pixels\n", PLACEMENT_TILE_DIST);
            src = {0, grid_begin_y, placement_bar_width-PLACEMENT_TILE_DIST, grid_height};
            dst = {PLACEMENT_TILE_DIST, grid_begin_y, placement_bar_width-PLACEMENT_TILE_DIST, grid_height};
            SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy shifted placement grid.
            draw_vertical_strip(_renderer, 0, PLACEMENT_TILE_DIST, grid_height);
            draw_next_placements(_renderer, next_placements, &src, &dst, scene_data);
            break;
        default:
            src = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy shifted placement grid.
            break;
    }

    // LOG(1, "INFO", "MapEditor::GetNextBackBuffer - Camera (x=%i, y=%i)\n", camera.x, camera.y);

    // >>> Copy over the rest of the pixels:
    // 1. Copy the right panel.
    src = {placement_bar_width, 0, SCREEN_WIDTH-placement_bar_width, SCREEN_HEIGHT};
    dst = {placement_bar_width, 0, SCREEN_WIDTH-placement_bar_width, SCREEN_HEIGHT};
    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy right panel.

    // 2. Copy over the top-menu.
    src = {0, 0, placement_bar_width, menu_y + placement_bar_y};
    dst = {0, 0, placement_bar_width, menu_y + placement_bar_y};
    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy top-menu.
    
    SDL_DestroyTexture(_back_buffer);                               // Now we can get rid of the back buffer.
    _common->SetBackBuffer(t);                                      // Swap to next back buffer.
}

void MapEditor::SwapBuffers() {
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_SetRenderTarget(_renderer, NULL);                            // Set the renderer to target the screen buffer now.
    SDL_RenderClear(_renderer);                                      // Un-apply the front-buffer.
    SDL_RenderCopy(_renderer, _common->GetBackBuffer(), NULL, NULL); // Apply the back buffer

    const SpriteSelection& entity = _sprite_selection;
    if (entity.selection) {
        BlitDragAndDrop(entity, _mouse_x, _mouse_y);
    }

    SDL_RenderPresent(_renderer);                                    // Present the front buffer.
}


MapEditor::MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {
    LOG(1, "TRACE", "MapEditor::cstr(common_ptr=?)\n");
    SceneData scene_data = GetFirstScene();
    common_ptr->NewScene(scene_data);
}

void HandleAddDragAndDrop(SceneData* scene_data, SpriteSelection* sprite_selection, std::shared_ptr<Common> common_ptr, const int mx, const int my) {
    LOG(1, "TRACE", "HandleAddAction(scene_data=%p, sprite_selection=?, common_ptr=?, mx=%i, my=%i)\n", scene_data, mx, my);

    bool acted_on = false;

    if (
        mx < SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH ||                               // To left of selection possible region.
        mx > (SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH) + SPRITESHEET_WIDTH/2 ||       // To right of selection possible region.
        my < SPRITE_TABLE_SELECTION_PANE_Y_OFFSET ||                                     // Above the Sprite table
        my > (SPRITE_TABLE_SELECTION_PANE_Y_OFFSET + SPRITESHEET_HEIGHT + SPRITE_HEIGHT) // Goes past Sprite Table + 'Special Row'
    )
    {
        LOG(1, "DEBUG", "Outside selection region, ignore attempt at selection.\n");
        return;
    }

    // Reference values to selection region.
    const uint16_t x_offset = SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH;
    const uint16_t y_offset = 64;



    // FIXME: This can be O(1) if we have an array s.t.
    //        [first_sprite, ..., end_row, first_next_row, ...]
    //        Then we'd just index into there depending on the X/Y given the origin at 0,0 (top-left of table)
    //        But for now, we just do a lazy search.
    auto MAPSPRITE_TAG = (SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
    auto PLAYER_TAG = (SPRITE_TAG | PLAYER_SPRITE_FLAG);

    ssize_t texture_idx = 0;

    for (uint8_t i = 0; i < SPRITE_TABLE_COLS; ++i) {
        for (uint8_t j = 0; j < SPRITE_TABLE_ROWS; ++j) {
            const uint16_t sprite_x = x_offset + (i*SPRITE_WIDTH);
            const uint16_t sprite_y = y_offset + (j*SPRITE_HEIGHT);
            if (mx > sprite_x && mx <= sprite_x + SPRITE_WIDTH && my > sprite_y && my <= sprite_y + SPRITE_HEIGHT) {
                // Store the drag&drop sprite for rendering later at mouse location.
                sprite_selection->selection = true;
                sprite_selection->x = i;
                sprite_selection->y = j;
                sprite_selection->tag = MAPSPRITE_TAG;
                LOG(1, "INFO", "Selection made on <MapSprite tag=%u, x=%u, y=%u>.\n", MAPSPRITE_TAG, i, j);
                return;
            }
        }
    }
    // Check for the player sprite selection
    if (mx < x_offset + SPRITE_WIDTH && my > y_offset + SPRITESHEET_HEIGHT) {
        sprite_selection->selection = true;
        sprite_selection->x = 0;
        sprite_selection->y = 0;
        sprite_selection->tag = PLAYER_TAG;
        LOG(1, "INFO", "Selection made on <PlayerSprite tag=%u>.\n", PLAYER_TAG);
        return;
    }

    if (!acted_on) LOG(1, "INFO", "Unable to act on add action, no sprite found to select\n");
}

void inline BlitRemovedPlacement(const ssize_t placement_x, const ssize_t placement_y) {
    LOG(1, "TRACE", "BlitRemovedPlacement(%u, %u)\n", placement_x, placement_y);
    SDL_Rect src = {placement_x+1, placement_y+1, 31, 31};
    SDL_Color color = {64,64,64,255};

    SDL_Renderer* _renderer = common_ptr->GetRenderer();
    SDL_SetRenderTarget(_renderer, common_ptr->GetBackBuffer()); // Blit to the back buffer.

    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(_renderer, &src);
}

void MapEditor::HandleSelection(const int mouse_x, const int mouse_y) {
    if (!(
        _editor_mode == editor_mode::ADD ||
        _editor_mode == editor_mode::DEL ||
        _editor_mode == editor_mode::MARK
     )) return;

    LOG(1, "TRACE", "MapEditor::HandleSelection( mouse_x=%i, mouse_y=%i )\n", mouse_x, mouse_y);

    SceneData* scene_data = _common->GetSceneData();

    if (mouse_x < SCREEN_WIDTH * 0.8 && _editor_mode == editor_mode::DEL) {
        int placement_idx = 0;
        bool placement_removed = false;
        {
            for(auto it = g_placements.begin(); it != g_placements.end(); ++it) {
                auto placement = (*it);
                int px = placement.x;
                int py = placement.y;
                if (mouse_x > px && mouse_x < px + 32 && mouse_y > py && mouse_y < py + 32) {
                    g_placements.erase(it);
                    BlitRemovedPlacement(px, py);
                    SwapBuffers();
                    placement_removed = true;
                    break;
                }
                placement_idx++;
            }
        }
        return;
    } else if (_editor_mode == editor_mode::ADD) {
        HandleAddDragAndDrop(scene_data, &_sprite_selection, _common, mouse_x, mouse_y);
    } else if (mouse_x < SCREEN_WIDTH * 0.8 && _editor_mode == editor_mode::MARK) {
        int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(0.05f);
        if (mouse_y < menu_y) return; // Disallow marking in the Menu Bar.

        Vector2D top_left_point = {
            .x = mouse_x - (mouse_x % SPRITE_WIDTH),
            .y = mouse_y - (mouse_y % SPRITE_HEIGHT)
        };

        // It's no good to mark a region with no placements, so let's check that.
        // binary_search_region on rows we aren't certain re: next index.
        bool was_matched = false;
        Vector2D vl = {
            .x = top_left_point.x,
            .y = top_left_point.y
        };

        Vector2D vr = {
            .x = top_left_point.x + (SPRITE_WIDTH*3),
            .y = top_left_point.y + (SPRITE_HEIGHT*3)
        };

        LOG_INFO("<TopLeftVec2D [%i, %i]>\n", vl.x, vl.y);
        LOG_INFO("<BottomRightVec2D [%i, %i]>\n", vr.x, vr.y);

        Placement top_left     = { .x = top_left_point.x, .y = top_left_point.y };
        Placement bottom_right = { .x = top_left_point.x + (SPRITE_WIDTH*3), .y = top_left_point.y + (SPRITE_HEIGHT*3) };
        auto it1 = g_placements.lower_bound(top_left);
        auto it2 = g_placements.upper_bound(bottom_right);

        for (auto it = it1; it != it2; ++it) {
            was_matched = true;
        }
        if (was_matched) {
            LOG_INFO("Matched on region\n");
            // It's a valid delineation in absence of overlap.
            // However, we can check overlaps since we have a dynamic array of Vector2D for the TL points.
            // If we sort this in O(nlogn), we can then do a binary search in it O(logn), meaning we can achieve O(n(logn^2)).
            // FIXME: Write library function to merge sort DynamicArray<Vector2D>

            // For now, we can quite trivially linear search this and just check boundary conditions.
            bool overlap = false;
            if (marked_maps->count > 0) {
                const Vector2D* items = *(marked_maps->items);
                for (int j = 0; j < marked_maps->count; ++j) {
                    Vector2D* vl = (marked_maps->items[j]);  // Existing delineated TL point.
                    Vector2D vr = {
                        .x = vl->x+(SPRITE_WIDTH*3),
                        .y = vl->y+(SPRITE_HEIGHT*3)
                    };
                    auto tl = top_left_point;
                    LOG_INFO("tl=[%i,%i] MATCH vl=[%i,%i], vr=[%i,%i]\n", tl.x, tl.y, vl->x, vl->y, vr.x, vr.y);
                    if (((tl.y <= vl->y && tl.y+(SPRITE_HEIGHT*3) >= vl->y) || // Y check
                         (tl.y >= vl->y && tl.y-(SPRITE_HEIGHT*3) <= vl->y)) &&
                        ((tl.x <= vl->x && tl.x+(SPRITE_WIDTH*3)  >= vl->x) || // X check
                         (tl.x >= vl->x && tl.x-(SPRITE_WIDTH*3)  <= vl->x))) {
                        LOG_INFO("=> Ignore due to overlap condition met.\n");
                        overlap = true;
                        break;
                    }
                }
            }
            if (!overlap) delineate_new_map_border(top_left_point.x, top_left_point.y);
            return;
        } else {
            LOG_INFO("No match on region\n");
            return;
        }
    }
}

void delineate_new_map_border(int top_left_x, int top_right_y) {
    LOG(1, "TRACE", "delineate_new_map_border(%i, %i)\n", top_left_x, top_right_y);
    Vector2D* v = arena_alloc(&temporary_arena, sizeof(Vector2D));
    v->x = top_left_x;
    v->y = top_right_y;
    arena_da_append(&temporary_arena, marked_maps, v);
    {
        const Vector2D inserted_vec2 = *(marked_maps->items[marked_maps->count-1]);
        assert(marked_maps->count > 0); // Not really necessary, checks if arena works as expected.
        LOG(1, "INFO", "Mark Map(.x=%i, .y=%i) - count: %d\n", inserted_vec2.x, inserted_vec2.y, marked_maps->count);
    }
    BlitMarkedMap(top_left_x, top_right_y);
}

static inline void process_rest_of_row(std::set<Placement, OrderHorizontally>::iterator& it, size_t oob_norm_col_x, std::vector<Placement>* tile) {
    LOG(1, "TRACE", "process_rest_of_row(?)\n");
    auto p = &(*it);
    auto y = p->y;
    tile->push_back(*p);
    LOG(1, "INFO", "Intend to store <Placement [%i,%i]>\n", p->x, p->y);
    for(++it; it != g_placements.end(); ++it) { // Iterate rest of row.
        p = &(*it);
        if (p->y > y) {
            break;
        }
        if (p->x/SPRITE_WIDTH >= oob_norm_col_x) { // Gone past the marked map end X.
            break;
        }
        tile->push_back(*p);
        LOG(1, "INFO", "Intend to store <Placement [%i,%i]\n", p->x, p->y);
    }
}

inline void debug_print_placements() {
    LOG(1, "TRACE", "debug_print_placements()\n");
    for (auto it = g_placements.begin(); it != g_placements.end(); ++it) {
        const Placement p = *it;
        LOG(1, "INFO", "Placement(.x=%i, .y=%i)\n", p.x, p.y);
    }
}

void handle_save_selection(Message* message) {
    LOG(1, "TRACE", "handle_save_selection()\n");
    if (marked_maps->count == 0) {
        *message = {
            .lines = {"Please mark a map."},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
        };
        return;
    }
    if (g_placements.size() == 0) {
        *message = {
            .lines = {"No placements found, invalid."},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
        };
        return;
    }
    std::vector<Placement> tile = {};
    uint8_t map_idx = 0;

    debug_print_placements();

    auto map_file = open_map_file();

    for (int j = 0; j < marked_maps->count; ++j) {
        // Marked map TL point.
        Vector2D* v = (marked_maps->items[j]);
        int x = v->x;
        int y = v->y;
        auto it = g_placements.find(Placement{.x = x, .y = y });
        if (it != g_placements.end()) {
            size_t oob_norm_col_x = x/SPRITE_WIDTH+4; // Out of bounds col X (normalized)
            process_rest_of_row(it, oob_norm_col_x, &tile);

            size_t row_idx = 1;
            int next_x = x;
            while (row_idx < 4) {
                Placement candidate = { .x = next_x, .y = y + SPRITE_HEIGHT*row_idx };
                it = g_placements.find(candidate);
                if (it != g_placements.end()) {
                    process_rest_of_row(it, oob_norm_col_x, &tile);
                    next_x = x;
                    row_idx++;
                    continue;
                }
                next_x+=SPRITE_WIDTH;
                if (next_x/SPRITE_WIDTH >= oob_norm_col_x) { // Gone past the marked map end X.
                    next_x = x;
                    row_idx++;
                    continue;
                }
            }
            save_map(tile, j, marked_maps->items[j], map_file); // j: map_idx
            tile.clear(); // Consumed.
        } else {
            LOG(1, "WARN", "Unable to find placements in marked map - (.x=%i, .y=%i)\n", x, y);
        }
    }

    finish_save_map(message, map_file);
}

const inline SDL_Texture* getAllocBackBuffer(std::shared_ptr<Common>& common_ptr) {
    return SDL_CreateTexture(
        common_ptr->GetRenderer(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        BACKBUFFER_WIDTH,
        BACKBUFFER_HEIGHT
    );
}

void MapEditor::BlitMessage(const Message& message) {
    LOG(1, "VERBOSE", "MapEditor::BlitMessage\n");
    assert(message.flushable); // FIXME: This is now a redundant flag.

    // BlitMessage:
    // 1. Copy across left-panel
    // 2. Copy across right-panel spritesheet
    // 3. Draw the actual text texture.

    SDL_Renderer* _renderer = _common->GetRenderer();
    const SDL_Texture* t = getAllocBackBuffer(_common);
    SDL_SetRenderTarget(_renderer, t);                              // Switch to our temporary back buffer holder texture.
    SDL_Texture* _back_buffer = _common->GetBackBuffer();

    int y_offset = 0.7f*SCREEN_HEIGHT; // FIXME: Rename this.

    SDL_Rect src;
    SDL_Rect dst;

    // 1. Copy across left-panel
    src = {0, 0, SCREEN_WIDTH-180, SCREEN_HEIGHT};
    dst = {0, 0, SCREEN_WIDTH-180, SCREEN_HEIGHT};
    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy left panel

    // 2. Copy across right-panel spritesheet
    src = {SCREEN_WIDTH-180, 0, 180, y_offset};
    dst = {SCREEN_WIDTH-180, 0, 180, y_offset};
    SDL_RenderCopy(_renderer, _back_buffer, &src, &dst);            // Copy spritesheet

    int line_offset = 0;

    if (message.flushable) {
        float y_offset = (SCREEN_HEIGHT * 0.8) + (SCREEN_HEIGHT * 0.05f);
        assert(message.lines.size() > 0);
        int message_offset_x = SCREEN_WIDTH - RIGHT_PANEL_WIDTH + (RIGHT_PANEL_WIDTH-message.line_width);
        for (const auto& line : message.lines) {
            LOG(1, "VERBOSE", "MapEditor::BlitMessage - (MessageLine: %s)\n", line.c_str());
            SDL_Surface* surface = TTF_RenderUTF8_Solid(
                get_font(_common, FONT_SIZE::SMALL),
                line.c_str(),
                {255,255,255,255}
            );
            // FIXME: Factor to Surface_Error();
            if (surface == NULL) {
                char errstr[256];
                SDL_GetErrorMsg(errstr, 256);
                LOG(1, "PANIC", "Failed to obtain surface - %s\n", errstr);
                SDL_Quit();
                exit(EXIT_FAILURE);
            }

            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(_renderer, surface);
            SDL_FreeSurface(surface);
            // FIXME: Factor to Texture_Failure();
            if (text_texture == NULL) {
                printf("Panic: Failed to create texture for text, abort.\n");
                exit(EXIT_FAILURE);
            }

            std::pair<int, int> texture_dims = GetTextureDimensions(text_texture);
            const int width = std::get<0>(texture_dims);
            const int height = std::get<1>(texture_dims);

            dst = {SCREEN_WIDTH-180, y_offset+line_offset, width, height};

            SDL_RenderCopy(_renderer, text_texture, NULL, &dst);

            line_offset+=height;
        }
    }

    SDL_DestroyTexture(_back_buffer);                               // Now we can get rid of the back buffer.
    _common->SetBackBuffer(t);                                      // Swap to next back buffer.
}

void MapEditor::HandleMenuBarSelection(const int mouse_x, const int mouse_y) {
    LOG(1, "TRACE", "MapEditor::HandleMenuBarSelection(mouse_x=%i, mouse_y=%i)\n", mouse_x, mouse_y);
    float menu_y_factor = 0.05f;
    int menu_y = static_cast<float>(SCREEN_HEIGHT)*static_cast<float>(menu_y_factor);

    Message message = {};

    if (mouse_y > menu_y) return;

    if (mouse_x > menu_item_offset && mouse_x < menu_item_offset + menu_item_width*1) {
        message = {
            .lines = {"ADD mode active!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
        _editor_mode = editor_mode::ADD;
    } else if (mouse_x > menu_item_offset + menu_item_width*1 && mouse_x < menu_item_offset + menu_item_width*2) {
        LOG(1, "INFO", "DEL menu item selected\n");
        _editor_mode = editor_mode::DEL;
        message = {
            .lines = {"DEL mode active!"},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
    } else if (mouse_x > menu_item_offset + menu_item_width*2 && mouse_x < menu_item_offset + menu_item_width*3) {
        handle_save_selection(&message);
    } else if (mouse_x > menu_item_offset + menu_item_width*3 && mouse_x < menu_item_offset + menu_item_width*4) {
        LOG(1, "INFO", "MARK menu item selected\n");
        _editor_mode = editor_mode::MARK;
        message = {
            .lines = {"MARK mode active !", "Please select top-left corner of 4x4 tile."},
            .word_wrap = true,
            .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH*(float)(RIGHT_PANEL_WIDTH))
        };
    }
    if (message.lines.size() > 0) BlitMessage(message);
}

void save_map(const vector<Placement>& tile, const uint8_t map_idx, const Vector2D* marked_map, std::ofstream& out_file) {
    LOG(1, "TRACE", "SaveMap(..., marked_map(.x=%i, .y=%i))\n", marked_map->x, marked_map->y);

    int player_begin_x = -1;
    int player_begin_y = -1;

    // Check if there's a player sprite inside this tile.
    for (auto it = tile.begin(); it != tile.end(); ++it) {
        const Placement p = *it;
        if (p.tag == player_sprite_tag) {
            player_begin_x = p.x;
            player_begin_y = p.y;
            LOG(1, "INFO", "SaveMap - Player(x=%i, y=%i)\n", player_begin_x, player_begin_y);
            break;
        }
    }

    out_file << "#define MAP" << std::to_string(map_idx) << "_METADATA " << std::to_string(tile[0].x) << "," << std::to_string(tile[0].y) << endl;

    if (player_begin_x != -1) {
        // MAP0_PLAYER_BEGIN_POSITION_X
        out_file << "#define MAP" << std::to_string(map_idx) << "_PLAYER_BEGIN_POSITION_X " << std::to_string(player_begin_x) << endl;
    }

    if (player_begin_y != -1) {
        // MAP0_PLAYER_BEGIN_POSITION_Y
        out_file << "#define MAP" << std::to_string(map_idx) << "_PLAYER_BEGIN_POSITION_Y " << std::to_string(player_begin_y) << endl;
    }

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
                LOG(1, "VERBOSE", "Pad rest of row\n");
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
                LOG(1, "INFO", "Pad complete rows\n");
                while(norm_y_delta > 1) {
                    out_file << "    {-1,-1,-1,-1},\\" << endl;
                    norm_y_delta--;
                }
            }

            out_file << "    {";

            // Pad beginning of row of placement.
            if (x_offset > 0) {
                LOG(1, "INFO", "Pad beginning of row\n");
            }
            while(x_offset > 0) {
                out_file << "-1,";
                x_offset--;
                col_cur++;
            }
        }

        // If the column cursor jumped, fill the gap.
        if (x_norm_jump > 1) {
            LOG(1, "INFO", "X cursor jumped, fill gap. (%i - %i)\n", norm_cur_x, norm_prev_x);
            while(x_norm_jump > 1) {
                out_file << "-1,";
                x_norm_jump--;
                col_cur++;
            }
        }

        // Arbitrary decision.
        // 1xx = Map Sprite
        // 2xx = Player Sprite(s)
        int encoding = (placement.sprite_x_idx * 16) + placement.sprite_y_idx;
        if (common_ptr->isBackgroundSpriteTexture(placement.tag)) {
            encoding += 100;
        }
        else if (common_ptr->isPlayerSpriteTexture(placement.tag)) {
            encoding += 200;
        }
        std::string sprite_encoding = std::to_string(encoding);
        LOG(1, "INFO", "Placement<x=%i, y=%i, encoding=%s>\n", placement.x, placement.y, sprite_encoding.c_str());
        out_file << sprite_encoding;
        if ((idx < tile.size() - 1 && tile[idx+1].y == tile[idx].y) || col_cur < 3) out_file << ",";
        col_cur++;
        idx++;
        prev_x = placement.x;
        prev_y = placement.y;
    }
    // Pad rest of row.
    if (col_cur < 4) {
        LOG(1, "VERBOSE", "Pad rest of row\n");
    }
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

    LOG(1, "INFO", "Need to pad finally %i rows\n", tile_y_delta);

    // Finished
    if (tile_y_delta == 0) out_file << "}\\" << endl;

    // Pad remaining rows.
    else {
        if (tile_y_delta > 0) LOG(1, "INFO", "Pad complete rows\n");
        out_file << "},\\" << endl;

        while(tile_y_delta > 0) {
            if (tile_y_delta != 1) out_file << "    {-1,-1,-1,-1},\\" << endl;
            else out_file << "    {-1,-1,-1,-1}\\" << endl;
            tile_y_delta--;
        }
    }

    out_file << "}" << endl;
}

void MapEditor::TryToPlace(const int mouse_x, const int mouse_y) {
    LOG(1, "INFO", "MapEditor::TryToPlace(mouse_x=%i, mouse_y=%i)\n", mouse_x, mouse_y);

    // Note: Selection cursor is top left of the sprite plus some padding.
    // Therefore it's best to take a "good guess" on what the sprite x/y is.

    const int sprite_x = mouse_x + 16; // Simulate mouse x as being centre x of the sprite.
    const int sprite_y = mouse_y + 16; // Simulate mouse y as being centre y of the sprite

    LOG(1, "INFO", "MapEditor::TryToPlace(?) - sprite_x=%i, sprite_y=%i\n", sprite_x, sprite_y);

    // Check if it can be placed in the Placement Area.
    if (sprite_x >= SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH || sprite_y < 64) return;

    // Snap grid starts at 64. No need to mod this value for now.

    Placement placement;

    // Adjust origin back to top-left of the sprite.
    placement.x = sprite_x - (sprite_x % 32);
    placement.y = sprite_y - (sprite_y % 32);

    LOG(1, "INFO", "Placement made at [%i,%i]\n", placement.x, placement.y);

    placement.sprite_x_idx = _sprite_selection.x;
    placement.sprite_y_idx = _sprite_selection.y;
    placement.tag = _sprite_selection.tag;
    placement.texture_idx = _sprite_selection.texture_idx;

    BlitNewPlacement(placement);
    SwapBuffers(); // Reflect immediately.

    store_placement(_common, placement);
    _sprite_selection.selection = false;
}

void MapEditor::UpdateMouseCoords(int x, int y) {
    LOG(1, "DEBUG", "Mouse(x=%i, y=%i)\n", x, y);
    _mouse_x = x;
    _mouse_y = y;
}

uint64_t frame_count = 0;
uint64_t current_ticks = 0;

static inline std::ifstream open_file(const char* file_name) {
    std::ifstream file(MAP_FILE);
    if (!file.is_open()) {
        LOG(1, "PANIC", "Failed to open previous map at file: %s\n", MAP_FILE);
        exit(EXIT_FAILURE);
    }
    return file;
}

static void inline parse_metadata(std::string& current_line, int* x, int* y) {
    current_line = current_line.substr(sizeof("#define MAP0_METADATA"));
    size_t pos = current_line.find(",");
    *x = atoi(current_line.substr(0, pos).c_str());
    current_line.erase(0, pos + 1);
    *y = atoi(current_line.c_str());
}


static inline constexpr void remove_spaces(char *s) {
    char *d = s;
    for (char *p = s; *p; ++p) {
        if (!isspace((unsigned char)*p)) {
            *d++ = *p;
        }
    }
    *d = '\0';
}

static size_t inline constexpr next_token_dist(std::string& current_line, char desired_token) {
    size_t dist = 0;
    for (;;) {
        char token = current_line[dist];
        if (token == '\0') {
            return 0;
        }
        else if (token == desired_token) {
            break;
        }
        dist++;
    }
    return dist;
}

static void inline parse_row(std::string& current_line, void* map, size_t* offset) {
    current_line.erase(std::remove(current_line.begin(), current_line.end(), '{'), current_line.end());
    current_line.erase(std::remove(current_line.begin(), current_line.end(), '}'), current_line.end());

    char first_token;
    remove_spaces(current_line.data());

    size_t dist;
    for (;;) {
        std::string prev_line = current_line; // DEBUG ONLY
        char first_token = current_line[0];
        dist = next_token_dist(current_line, ',');
        if (dist == 0) {
            dist = next_token_dist(current_line, '\\');
            if (dist == 0) {
                LOG(1, "WARN", "(dist=%d, line=%s, line_size=%d)\n", dist, current_line.c_str(), current_line.size());
                break;
            }
        }
        const char* chopped_value = current_line.substr(0, dist).c_str();
        current_line.erase(0, dist);
        int sprite_encoding = atoi(chopped_value);
        LOG(1, "INFO", "(chop=%s, dist=%d, encoding=%i, first_token=%c)\n", chopped_value, dist, sprite_encoding, first_token);
        std::memcpy(map + *offset, &sprite_encoding, sizeof(sprite_encoding));
        *offset += sizeof(int32_t); // Move along by int32_t
        current_line.erase(0, 1); // Drop the comma
    }
}

void inline parse_player_begin_position_x(std::string& header, std::string& current_line, int* player_begin_x) {
    LOG(1, "TRACE", "parse_player_begin_position_x(header=%s, line=%s, x=%p)\n", header.c_str(), current_line.c_str(), player_begin_x);
    current_line = current_line.substr(header.size());
    LOG(1, "INFO", "Zeroth chop: %s\n", current_line.c_str());
    const char* chop_first_value = current_line.substr(0, current_line.size() - header.size()).c_str();
    *player_begin_x = atoi(chop_first_value);
    LOG(1, "INFO", "parse_player_begin_position - (chop_first=%s, x=%i)\n", chop_first_value, *player_begin_x);
}

void inline parse_player_begin_position_y(std::string& header, std::string& current_line, int* player_begin_y) {
    LOG(1, "TRACE", "parse_player_begin_position_y(header=%s, line=%s, y=%p)\n", header.c_str(), current_line.c_str(), player_begin_y);
    current_line = current_line.substr(header.size());
    LOG(1, "INFO", "Zeroth chop: %s\n", current_line.c_str());
    const char* chop_first_value = current_line.substr(0, current_line.size() - header.size()).c_str();
    *player_begin_y = atoi(chop_first_value);
    LOG(1, "INFO", "parse_player_begin_position - (chop_first=%s, y=%i)\n", chop_first_value, *player_begin_y);
}

std::string inline get_metadata_line_header(size_t map_idx) {
    return "#define MAP" + std::to_string(map_idx) + "_METADATA";
}

std::string inline get_row_line_header(size_t map_idx) {
    return "#define MAP" + std::to_string(map_idx);
}

std::string inline get_player_begin_x_header(size_t map_idx) {
    return "#define MAP" + std::to_string(map_idx) + "_PLAYER_BEGIN_POSITION_X";
}

std::string inline get_player_begin_y_header(size_t map_idx) {
    return "#define MAP" + std::to_string(map_idx) + "_PLAYER_BEGIN_POSITION_Y";
}

void MapEditor::Load() {
    LOG(1, "TRACE", "Load()\n");

    std::string line;
    size_t line_number = 0;
    int map_begin_x, map_begin_y;
    int player_begin_x, player_begin_y;

    size_t map_size = sizeof(int) * 16;

    void* map = arena_alloc(&default_arena, map_size);
    void* begin_map = map;

    size_t offset = 0;
    size_t map_idx = 0;

    std::string metadata_line_header = get_metadata_line_header(map_idx);
    std::string row_line_header = get_row_line_header(map_idx);
    std::string player_begin_x_header = get_player_begin_x_header(map_idx);
    std::string player_begin_y_header = get_player_begin_y_header(map_idx);

    std::ifstream file = open_file(MAP_FILE);
    for (;;) {
        if (!std::getline(file, line)) break;
        if (line.starts_with("}")) break; // FIXME: We need to be able to handle >1 map.

        LOG(1, "INFO", "Line: %s\n", line.c_str());

        if (line.starts_with(metadata_line_header)) {
            parse_metadata(line, &map_begin_x, &map_begin_y);
            LOG(1, "INFO", "TL: [%i,%i]\n", map_begin_x, map_begin_y);
            continue;
        }

        if (line.starts_with(player_begin_x_header)) {
            parse_player_begin_position_x(player_begin_x_header, line, &player_begin_x);
        }
        if (line.starts_with(player_begin_y_header)) {
            parse_player_begin_position_y(player_begin_y_header, line, &player_begin_y);
            LOG(1, "INFO", "Player Begin Position: [%i,%i]\n", player_begin_x, player_begin_y);
            continue;
        }

        if (line.starts_with(row_line_header)) {
            map_idx++;
            metadata_line_header = get_metadata_line_header(map_idx);
            row_line_header = get_row_line_header(map_idx);
            continue;
        }

        parse_row(line, map, &offset);
    }
    file.close();

    _prev_map.first_tile_x = map_begin_x;
    _prev_map.first_tile_y = map_begin_y;

    size_t end = begin_map + sizeof(int) * 16;

    LOG(1, "INFO", "begin_map_ptr: %p\n", begin_map);
    LOG(1, "INFO", "map_ptr: %p\n", map);

    std::vector<std::vector<int>> tiles(4, std::vector<int>(4));

    size_t tile_idx = 0;
    size_t counter = 0;

    while(begin_map < end) {
        const int* encoding = static_cast<const int*>(begin_map);
        LOG(1, "INFO", "encoding: %i\n", *encoding);
        auto row_idx = counter%4;
        tiles[tile_idx][row_idx] = *encoding;
        begin_map+=sizeof(int);
        counter++;
        tile_idx = counter/4;
    }
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
    uint8_t placements_restored = 0;
    uint8_t tag;

    for (const auto& tile : _prev_map.tiles) {
        for (auto v : tile) {
            if (v >= 100 && v < 200) {
                tag = (SPRITE_TAG | BACKGROUND_SPRITE_FLAG);
                v-=100;
            }
            else if (v >= 200) {
                tag = (SPRITE_TAG | PLAYER_SPRITE_FLAG);
                v-=200;
            }
            else if (v < -1 || v > 300) {
                LOG(1, "PANIC", "Unexpected encoding: %i\n", v);
                ASSERT_NOT_REACHED();
            }

            if (v != -1 && !first_placement_set) {
                placement.sprite_x_idx = v / 16;
                placement.sprite_y_idx = v % 16;
                placement.tag = tag;
                x_offset_idx = x_idx;
                first_placement_set = true;
                store_placement(_common, placement);
                placements_restored++;
            } else if (v != -1) {
                const int x_offset = x_idx - x_offset_idx;
                const int y_offset = y_idx - y_offset_idx;
                Placement p = Placement {
                    .x = _prev_map.first_tile_x + (x_offset * 32),
                    .y = _prev_map.first_tile_y + (y_offset * 32),
                    .sprite_x_idx = v / 16,
                    .sprite_y_idx = v % 16,
                    .tag = tag,
                };
                store_placement(_common, p);
                placements_restored++;
            }
            x_idx++;
        }
        y_idx++;
        x_idx = 0;
    }
    LOG(1, "INFO", "Restored %u placements.\n", placements_restored);
}

void MapEditor::FillBackBufferInitial() {
    LOG(1, "TRACE", "MapEditor::FillBackBufferInitial\n");
    SDL_Renderer* _renderer = _common->GetRenderer();
    SDL_Texture* _back_buffer = _common->GetBackBuffer();
    SDL_SetRenderTarget(_renderer, _back_buffer);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    uint8_t texture_idx = 0;
    uint8_t color_idx = 0;

    Entity probe_low;
    Entity probe_high;

    SceneData scene_data = common_ptr->scene_data[0];

    auto [it_rect, end_it_rect] = scene_data.entities.equal_range(RECT_TAG);

    for (; it_rect != end_it_rect; ++it_rect) {
        const Entity entity = it_rect->second;
        LOG(1, "INFO", "rect tag entity => %s\n", entity.tag == RECT_TAG ? "yes" : "no");
        SDL_Rect src_rect = entity.src_rect;
        SDL_Color color = entity.color;
        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(_renderer, &src_rect);
    }

    auto [it_text, end_it_text] = scene_data.entities.equal_range(TEXT_TAG);

    for (; it_text != end_it_text; ++it_text) {
        const Entity entity = it_text->second;
        SDL_Rect dst_rect = entity.dst_rect;
        LOG(1, "INFO", "<Text tag=%u, dst_rect<y=%u, y=%i, w=%i, h=%i>>\n", entity.tag, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h);
        SDL_RenderCopy(_renderer, entity.data, NULL, &dst_rect);
    }

    LOG(1, "INFO", "Background sprite tag: %u\n", bg_sprite_tag);
    auto [it_bg_sprite, end_it_bg_sprite] = scene_data.entities.equal_range(bg_sprite_tag);

    for (; it_bg_sprite != end_it_bg_sprite; ++it_bg_sprite) {
        const Entity entity = it_bg_sprite->second;
        LOG(1, "INFO", "bg_sprite tag entity => %s\n", entity.tag == bg_sprite_tag ? "yes" : "no");
        SDL_Rect src_rect;
        SDL_Rect dst_rect;
        
        // Get the spritesheet texture.
        SDL_Texture* texture = entity.data;

        for (auto it = g_placements.begin(); it != g_placements.end(); ++it) {
            Placement p = *it;
            if (_common->isPlayerSpriteTexture(p.tag)) {
                continue;
            }
            src_rect.x = p.sprite_x_idx*16;
            src_rect.y = p.sprite_y_idx*16;
            src_rect.w = 16;
            src_rect.h = 16;
            dst_rect.x = p.x;
            dst_rect.y = p.y;
            dst_rect.w = 32;
            dst_rect.h = 32;
            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
        }

            // Render the spritesheet to the right.
            const uint16_t x_offset = SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH;
            const uint16_t y_offset = SPRITE_TABLE_SELECTION_PANE_Y_OFFSET;

            src_rect.x = 0;
            src_rect.y = 0;
            src_rect.w = SPRITESHEET_WIDTH;
            src_rect.h = SPRITESHEET_HEIGHT;
            dst_rect.x = x_offset;
            dst_rect.y = y_offset;
            dst_rect.w = SPRITESHEET_WIDTH;
            dst_rect.h = SPRITESHEET_HEIGHT;

            SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);
    }

    LOG(1, "INFO", "Background sprite tag: %u\n", player_sprite_tag);
    auto [it_player_sprite, end_it_player_sprite] = scene_data.entities.equal_range(player_sprite_tag);

    for (; it_player_sprite != end_it_player_sprite; ++it_player_sprite) {
        const Entity entity = it_player_sprite->second;
        LOG(1, "INFO", "player_sprite tag entity => %s\n", entity.tag == player_sprite_tag ? "yes" : "no");
        SDL_Rect src_rect;
        SDL_Rect dst_rect;

        SDL_Texture* texture = entity.data;
        
        const uint16_t x_offset = SCREEN_WIDTH - SPRITE_SELECTION_PANE_WIDTH;
        const uint16_t y_offset = SPRITESHEET_HEIGHT + SPRITE_TABLE_SELECTION_PANE_Y_OFFSET;

        src_rect.x = 0;
        src_rect.y = 0;

        dst_rect.x = x_offset;
        dst_rect.y = y_offset;
        dst_rect.w = SPRITE_WIDTH;
        dst_rect.h = SPRITE_HEIGHT;

        SDL_RenderCopy(_renderer, texture, &src_rect, &dst_rect);

        // Render placements.
        for (auto it = g_placements.begin(); it != g_placements.end(); ++it) {
            Placement p = *it;
            if (!_common->isPlayerSpriteTexture(p.tag)) {
                continue;
            }
            src_rect.x = 0;
            src_rect.y = 0;
            src_rect.w = 50;
            src_rect.h = 50;
            dst_rect.x = p.x;
            dst_rect.y = p.y;
            dst_rect.w = 32;
            dst_rect.h = 32;
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
    while(line_y_offset < SCREEN_HEIGHT) {
        SDL_RenderDrawLine(_renderer, 0, line_y_offset, SCREEN_WIDTH-192, line_y_offset);
        line_y_offset+=32;
    }
    
    auto vertical_line_begin_y = 64;
    
    // Draw's the vertical lines.
    while(line_x_offset < SCREEN_WIDTH-180) {
        SDL_RenderDrawLine(_renderer, line_x_offset, vertical_line_begin_y, line_x_offset, line_y_offset - 32);
        line_x_offset+=32;
    }
    
    // Render the marked maps.
    for (int i = 0; i < marked_maps->count; ++i) {
        const Vector2D* v = marked_maps->items[i];
        SDL_SetRenderDrawColor( _renderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderDrawLine( _renderer, v->x, v->y, v->x+32*4, v->y);           // top line
        SDL_RenderDrawLine( _renderer, v->x, v->y, v->x, v->y+32*4);           // left line
        SDL_RenderDrawLine( _renderer, v->x+32*4, v->y, v->x+32*4, v->y+32*4); // right line
        SDL_RenderDrawLine( _renderer, v->x, v->y+32*4, v->x+32*4, v->y+32*4); // Bottom line
    
        SDL_RenderDrawLine( _renderer, v->x+1, v->y, v->x+32*4+1, v->y);             // top line
        SDL_RenderDrawLine( _renderer, v->x, v->y, v->x, v->y+32*4+1);               // left line
        SDL_RenderDrawLine( _renderer, v->x+32*4+1, v->y, v->x+32*4+1, v->y+32*4+1); // right line
        SDL_RenderDrawLine( _renderer, v->x, v->y+32*4+1, v->x+32*4+1, v->y+32*4+1); // Bottom line
    }
}

static Uint64 tick = SDL_GetTicks64();     // SDL Library is initialized above via Common::Common()
static Uint64 last_frame_tick = tick - 17; // Ensures we always render the first frame straight-away.
static Uint64 dt_frame = 17;               // Ensures no initial event wait.
static Uint64 frame_per_second = 0;        // Counter that gets reset after 1s.
static Uint64 seconds = 1;                 // Seconds rendered.

void move_placement_area(uint8_t direction) {
    LOG(1, "TRACE", "MapEditor::placement_move(%u)\n", direction);
    map_editor->BlitPlacementArea(direction);
}

void initialize_the_mapeditor() {
    LOG(1, "TRACE", "initialize_the_mapeditor()\n");
    marked_maps = arena_alloc(&default_arena, sizeof(Marked_Maps));
    common_ptr = std::make_shared<Common>("MapEditor", BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
    map_editor = std::make_unique<MapEditor>(common_ptr);

    Message message = {
        .lines = {"No mode currently active."},
        .word_wrap = true,
        .line_width = (unsigned int)(MESSAGE_RIGHT_PANEL_WIDTH * (float)(RIGHT_PANEL_WIDTH))
    };
    map_editor->BlitMessage(message);
    SetMapFile();
    map_editor->Load();
    map_editor->FillBackBufferInitial();
    map_editor->SwapBuffers();
    LOG(1, "INFO", "camera(x=%i, y=%i)\n", camera.x, camera.y);
}

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
        else if (eh.type == SDL_KEYDOWN) {
            auto sym = eh.key.keysym.sym;
            if (sym == SDLK_UP) {
                move_placement_area(PLACEMENT_MOVE_GRID_UP);
            }
            else if (sym == SDLK_DOWN) {
                move_placement_area(PLACEMENT_MOVE_GRID_DOWN);
            }
            else if (sym == SDLK_LEFT) {
                move_placement_area(PLACEMENT_MOVE_GRID_LEFT);
            }
            else if (sym == SDLK_RIGHT) {
                move_placement_area(PLACEMENT_MOVE_GRID_RIGHT);
            }
        }
        else if (eh.type == SDL_MOUSEMOTION) {
            map_editor->UpdateMouseCoords(eh.motion.x, eh.motion.y);
            interactive = true;
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
                // Drag & Drop before, now an active input to place it.
                map_editor->TryToPlace(x, y);
            }
        }
    }

    tick = SDL_GetTicks64();
    dt_frame = tick - last_frame_tick;
    if (dt_frame > DEFAULT_WAIT && interactive) { // Allow 2ms draw time.
        Uint64 before = SDL_GetTicks64();

        map_editor->SwapBuffers();

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

// FIXME: Now we can move the placement grid:
//        1. We should advance by "Placement Grid square" on up/down/left/right
//        2. Draw the next part of the placement grid.

// FIXME: The player position should be respected s.t. the Map header file does indeed define the player start point.
