#pragma once

#include "Panel.h"

#include <unordered_map>

namespace geck {

class Map;

/**
 * UI widget for displaying properties from the current MAP file
 */
class MapInfoPanel : public Panel {
public:
    MapInfoPanel(Map* map);

    void render(float dt) override;

private:
    void loadScriptVars();

    Map* _map;
    std::string _mapScriptName{ "no script" };

    //    std::unordered_map<std::string, uint32_t> _gvars;
    //    std::unordered_map<std::string, uint32_t> _lvars;
    std::unordered_map<std::string, uint32_t> _mvars;
};

} // namespace geck
