#pragma once

#include "Panel.h"

#include "format/map/Map.h"
#include "util/Signal.h"

namespace geck {

class MainMenuPanel : public Panel {
public:
    MainMenuPanel(Map* map, int currentElevation);

    void render(float dt) override;

    Signal<> menuNewMapClicked;
    Signal<> menuSaveMapClicked;
    Signal<> menuLoadMapClicked;
    Signal<> menuQuitMapClicked;

    Signal<bool> menuShowObjectsClicked;
    Signal<bool> menuShowCrittersClicked;
    Signal<bool> menuShowWallsClicked;
    Signal<bool> menuShowRoofsClicked;
    Signal<bool> menuShowScrollBlkClicked;

    Signal<int> menuElevationClicked;

    Signal<Map*> editorMapLoaded;

private:
    int _currentElevation;

    bool _showObjects = true;
    bool _showCritters = true;
    bool _showWalls = true;
    bool _showRoof = true;
    bool _showScrollBlk = true;

    Map* _map;
};

} // namespace geck
