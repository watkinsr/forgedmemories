#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>
#include <string>

typedef struct {
    int x;
    int y;
} Vector2D;

struct Message {
    std::vector<std::string> lines = {};
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

typedef struct {
    const Vector2D **items;
    size_t count;
    size_t capacity;
} Marked_Maps;

#endif
