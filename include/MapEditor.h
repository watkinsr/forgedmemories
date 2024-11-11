#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "Log.h"
#include "Common.h"

using namespace std;

class MapEditor {
public:
    MapEditor(std::shared_ptr<Common> common_ptr);
    ~MapEditor() {}
    void RenderCurrentScene();
    void _SetTextureLocations();
private:
    std::shared_ptr<Common> _common;
};

#endif
