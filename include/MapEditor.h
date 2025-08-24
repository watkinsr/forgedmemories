#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <set>

#include <fstream>

#include <cassert>
#include <cstring>
#include <limits.h>    // for PATH_MAX
#include <unistd.h>

#include "Log.h"
#include "Common.h"
#include "Types.h"

#define STEP_SIZE 10

const int BACKBUFFER_WIDTH = 800;
const int BACKBUFFER_HEIGHT = 600;

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

static constexpr void SetMapFile() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        perror("getcwd() error");
    }

    strcpy(MAP_FILE, cwd);
    strcat(MAP_FILE, "/include/Map.h");
    LOG(1, "INFO", "Map file: %s\n", MAP_FILE);
}

struct Placements {
    uint8_t tag;
    std::vector<Placement> data;
};

std::set<Placement, OrderHorizontally> g_placements;

class MapEditor {
public:
    MapEditor(std::shared_ptr<Common> common_ptr);
    ~MapEditor() {}
    void BlitPlacementArea(uint8_t direction);
    void BlitMessage(const Message& message);
    void BlitNext(uint8_t direction);
    void SwapBuffers();
    void FillBackBufferInitial();
    void GetNextBackBuffer(SDL_Texture* t, uint8_t direction);
    void _SetTextureLocations();
    void HandleSelection(const int, const int);
    void HandleMenuBarSelection(const int, const int);
    void UpdateMouseCoords(int, int);
    bool isSelectionActive() { return _sprite_selection.selection; }
    void TryToPlace(int, int);
    void save_tile(const std::vector<Placement>&, const uint8_t);
    void set_fps(const uint8_t fps) { _fps = fps; };
    void Load();
    Timepoint prev_clock = {};
    SpriteSelection _sprite_selection;
private:
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
