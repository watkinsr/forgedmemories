#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "Log.h"
#include "Common.h"

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

class MapEditor {
public:
    MapEditor(std::shared_ptr<Common> common_ptr);
    ~MapEditor() {}
    void RenderCurrentScene();
    void _SetTextureLocations();
    void HandleSelection(const int, const int);
    void UpdateMouseCoords(int, int);
    bool isSelectionActive() { return _sprite_selection.selection; }
    void TryToPlace(int, int);
private:
    std::shared_ptr<Common> _common;
    SpriteSelection _sprite_selection;
    std::vector<Placement> _placements;
    float _prev_tick;
    int _mouse_x;
    int _mouse_y;
};

#endif
