#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>
#include <string>

struct Entity {
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
    uint8_t tag;
    SDL_Texture* data;
};

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
    uint16_t x;   // Index into relevant spritesheet row
    uint16_t y;   // index into relevant spritesheet column
    uint8_t tag;  // Is it a Player sprite or Generic Map sprites or anything else?
    uint8_t texture_idx;
    Entity* entity_ptr; // Ptr -> SceneData::v
    bool selection = false;
};

struct Placement {
    uint16_t x;
    uint16_t y;
    uint16_t sprite_x_idx;
    uint16_t sprite_y_idx;
    uint8_t texture_idx;
    uint8_t tag; // Is it a Player sprite or Generic Map sprites or anything else?
};

struct prev_map_t {
    int first_tile_x;
    int first_tile_y;
    std::vector<std::vector<int>> tiles;
};

enum editor_mode {
    EDITOR_SENTINEL,
    ADD,
    DEL,
    MARK
};

typedef struct {
    const Vector2D **items;
    size_t count;
    size_t capacity;
} Marked_Maps;

struct OrderHorizontally {
    bool operator()(const Placement& a, const Placement& b) const {
        return a.y < b.y || (a.y == b.y && a.x < b.x);
    }
};

struct OrderEntity {
    bool operator()(const Entity& a, const Entity& b) const {
        if (a.tag != b.tag) return a.tag < b.tag;
        if (a.dst_rect.y != b.dst_rect.y) return a.dst_rect.y < b.dst_rect.y;
        if (a.dst_rect.x != b.dst_rect.x) return a.dst_rect.x < b.dst_rect.x;
        return a.data < b.data; // last tie-breaker
    }
};

#endif
