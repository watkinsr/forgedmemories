#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <fstream>

#include <unistd.h>
#include <limits.h>    // for PATH_MAX
#include <cassert>
#include <cstring>

#include "Log.h"
#include "Common.h"
#include "Types.h"

using namespace std;

using Timepoint = chrono::time_point<chrono::steady_clock>;

const size_t SPRITE_WIDTH = 32;
const size_t SPRITE_HEIGHT = 32;

constexpr uint8_t PLAYER_WIDTH = 48;
constexpr uint8_t PLAYER_HEIGHT = 48;

const int menu_item_width = 70;  //px
const int menu_item_offset = 25; //px

const int RIGHT_PANEL_WIDTH = 180;
const float MESSAGE_RIGHT_PANEL_WIDTH = 0.90f;

static char MAP_FILE[128];
bool g_map_file_set = false;

static void SetMapFile() {
    // Expectation to call only once.
    assert(!g_map_file_set);

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        perror("getcwd() error");
    }

    strcpy(MAP_FILE, cwd);
    strcat(MAP_FILE, "/include/Map.h");
    LOG(1, "INFO", "Map file: %s\n", MAP_FILE);
    g_map_file_set = true;
}

struct Placements {
    uint8_t tag;
    std::vector<Placement> data;
};

Placements g_generic_map_placements = { .tag = (SPRITE_TAG | BACKGROUND_SPRITE_FLAG), .data = {} };
Placements g_player_placements = { .tag = (SPRITE_TAG | PLAYER_SPRITE_FLAG), .data = {} };

class MapEditor {
public:
    MapEditor(std::shared_ptr<Common> common_ptr);
    ~MapEditor() {}
    void RenderCurrentScene();
    void _SetTextureLocations();
    void HandleSelection(const int, const int);
    void HandleMenuBarSelection(const int, const int);
    void UpdateMouseCoords(int, int);
    bool isSelectionActive() { return _sprite_selection.selection; }
    void TryToPlace(int, int);
    void save_tile(const std::vector<Placement>&, const uint8_t);
    void set_fps(const uint8_t fps) { _fps = fps; };
    void TryLoadPreviousMap();
    Timepoint prev_clock = {};
    Message message = {};
private:
    SpriteSelection _sprite_selection;
    std::shared_ptr<Placement> _player_placement = nullptr;
    editor_mode _editor_mode = editor_mode::SENTINEL;  // Default state - no mode.
    prev_map_t _prev_map;

    std::shared_ptr<Common> _common;

    // Miscellaneous fields.
    int _messages_flushed = 0;
    float _prev_tick;
    int _mouse_x;
    int _mouse_y;
    uint8_t _fps = 0;
};

#endif
