#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "Log.h"
#include "Common.h"
#include <fstream>

using namespace std;

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

std::string MAP_FILE = "/home/ryanwatkins/workplace/rom/include/Map.h";

struct prev_map_t {
    int first_tile_x;
    int first_tile_y;
    std::vector<std::vector<int>> tiles;
};

enum editor_mode {
    ADD,
    DEL
};

const int menu_item_width = 55; //px
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
private:
    std::shared_ptr<Common> _common;
    SpriteSelection _sprite_selection;
    std::vector<Placement> _placements;
    std::vector<std::string> _messages;
    editor_mode _editor_mode = editor_mode::ADD;
    int _messages_flushed = 0;
    prev_map_t _prev_map;
    float _prev_tick;
    int _mouse_x;
    int _mouse_y;
    uint8_t _fps = 0;
};

#endif
