#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "Log.h"
#include "Common.h"

using namespace std;

class MapEditor {
public:
    MapEditor(std::shared_ptr<Common> common_ptr) : _common(common_ptr) {}
    ~MapEditor() {}
    void RenderCurrentScene();
private:
    std::shared_ptr<Common> _common;
};

#endif
