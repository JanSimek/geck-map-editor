#include "MapInfoPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

#include "format/gam/Gam.h"
#include "format/lst/Lst.h"
#include "format/map/Map.h"
#include "reader/lst/LstReader.h"
#include "reader/gam/GamReader.h"
#include "util/ResourceManager.h"

namespace geck {

MapInfoPanel::MapInfoPanel(Map* map)
    : _map(map) {
    _title = "Map Information";

    loadScriptVars();
}

void MapInfoPanel::render(float dt) {
    auto mapInfo = _map->getMapFile();
    int elevations = mapInfo.tiles.size();

    ImGui::Begin(ICON_FA_MAP_MARKER_ALT " Map Information");

    const auto addInputScalar = [](const auto& label, const auto& value) {
        ImGui::InputScalar(label, ImGuiDataType_U32, value, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
    };

    constexpr float left_column_width = 0.35f;

    if (ImGui::CollapsingHeader("Map header")) {

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * left_column_width);
        ImGui::LabelText("Property", "Value");

        ImGui::InputText("Filename", &mapInfo.header.filename, ImGuiInputTextFlags_ReadOnly);
        addInputScalar("Map elevations", &elevations);
        addInputScalar("Player default position", &mapInfo.header.player_default_position);
        addInputScalar("Player default elevation", &mapInfo.header.player_default_elevation);
        addInputScalar("Player default orientation", &mapInfo.header.player_default_orientation);
        addInputScalar("Global variables #", &mapInfo.header.num_global_vars);
        addInputScalar("Local variables #", &mapInfo.header.num_local_vars);
        ImGui::InputText("Map script", &_mapScriptName, ImGuiInputTextFlags_ReadOnly);
        addInputScalar("Map script ID", &mapInfo.header.script_id);
        addInputScalar("Darkness", &mapInfo.header.darkness);
        addInputScalar("Map ID", &mapInfo.header.map_id);
        addInputScalar("Timestamp", &mapInfo.header.timestamp);
        bool isSavegame = ((mapInfo.header.flags & 0x1) != 0);
        ImGui::Checkbox("Save game map", &isSavegame);
    }

    if (ImGui::CollapsingHeader("Map global variables")) {

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * left_column_width);
        ImGui::LabelText("Variable", "Value");

        for (auto& [key, value] : _mvars) {
            addInputScalar(key.c_str(), &value);
        }
    }

    if (ImGui::CollapsingHeader("Map scripts")) {
        // TODO
    }

    ImGui::End(); // Map Information
}

void MapInfoPanel::loadScriptVars() {

    auto gam_filename = _map->filename().substr(0, _map->filename().find(".")) + ".gam";

    GamReader gam_reader{};
    auto gam_file = ResourceManager::getInstance().loadResource(std::filesystem::path("maps") / gam_filename, gam_reader);

    // Global variables
    for (int index = 0; index < _map->getMapFile().header.num_global_vars; index++) {
        _mvars.emplace(gam_file->mvarKey(index), gam_file->mvarValue(index));
    }

    int map_script_id = _map->getMapFile().header.script_id;
    if (map_script_id > 0) {
        LstReader lst_reader{};
        auto scripts = ResourceManager::getInstance().loadResource("scripts/scripts.lst", lst_reader);
        _mapScriptName = scripts->at(map_script_id - 1); // script id starts at 1
    }
}

} // namespace geck
