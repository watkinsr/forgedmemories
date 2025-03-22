#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <fstream>

#include <unistd.h>
#include <limits.h>    // for PATH_MAX
#include <cassert>
#include <cstring>

#include "Log.h"
#include "Common.h"

using namespace std;

using Timepoint = chrono::time_point<chrono::steady_clock>;

const size_t SPRITE_WIDTH = 32;
const size_t SPRITE_HEIGHT = 32;

struct Message {
    std::vector<string> lines = {};
    bool word_wrap = true;
    bool flushable = true;
    unsigned int line_width = 0;
};

struct SpriteSelection {
    uint16_t x; // index into spritesheet row
    uint16_t y; // index into spritesheet column
    bool selection = false;
};

struct Placement {
    uint16_t x;
    uint16_t y;
    uint16_t sprite_x_idx;
    uint16_t sprite_y_idx;
};

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

struct prev_map_t {
    int first_tile_x;
    int first_tile_y;
    std::vector<std::vector<int>> tiles;
};

enum editor_mode {
    SENTINEL,
    ADD,
    DEL,
    MARK
};

const int menu_item_width = 70;  //px
const int menu_item_offset = 25; //px

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
    std::shared_ptr<Common> _common;
    SpriteSelection _sprite_selection;
    std::vector<Placement> _placements;
    editor_mode _editor_mode = editor_mode::SENTINEL;  // Default state - no mode.
    int _messages_flushed = 0;
    prev_map_t _prev_map;
    float _prev_tick;
    int _mouse_x;
    int _mouse_y;
    uint8_t _fps = 0;
};

#endif
