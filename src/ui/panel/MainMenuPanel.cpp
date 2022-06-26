#include "MainMenuPanel.h"

#include <imgui.h>
#include <memory>
#include "ui/IconsFontAwesome5.h"
#include "ui/util.h"
#include "state/LoadingState.h"
#include "state/EditorState.h"
#include "state/loader/MapLoader.h"

namespace geck {

MainMenuPanel::MainMenuPanel(Map* map, int currentElevation)
    : _map(map)
    , _currentElevation(currentElevation) {
    _title = "##MainMenu";
}

void MainMenuPanel::render(float dt) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE " New map", "Ctrl+N")) {
                menuNewMapClicked.emit();
            }
            if (ImGui::MenuItem(ICON_FA_SAVE " Save map", "Ctrl+S")) {
                menuSaveMapClicked.emit();
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open map", "Ctrl+O")) {
                menuLoadMapClicked.emit();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                menuQuitMapClicked.emit();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            bool disabled = true;

            if (ImGui::MenuItemCheckbox("Show objects", &_showObjects)) {
                menuShowObjectsClicked.emit(_showObjects);
            }

            if (ImGui::MenuItemCheckbox("Show critters", &_showCritters, disabled)) {
                menuShowCrittersClicked.emit(_showCritters);
            }

            if (ImGui::MenuItemCheckbox("Show walls", &_showWalls, disabled)) {
                menuShowWallsClicked.emit(_showWalls);
            }

            if (ImGui::MenuItemCheckbox("Show roofs", &_showRoof)) {
                menuShowRoofsClicked.emit(_showRoof);
            }

            if (ImGui::MenuItemCheckbox("Show scroll block", &_showScrollBlk)) {
                menuShowScrollBlkClicked.emit(_showScrollBlk);
            }

            ImGui::Separator();
            ImGui::Tooltip("Currently visible map elevation");
            ImGui::SameLine();
            if (ImGui::BeginMenu("Elevation")) {
                if (_map->elevations() == 1) {
                    ImGui::Text("Map has only 1 elevation");
                } else {
                    for (int elevation = 0; elevation < _map->elevations(); elevation++) {
                        std::string text = std::to_string(elevation) + (elevation == _currentElevation ? " " ICON_FA_CHECK_CIRCLE : "");
                        if (ImGui::MenuItem(text.c_str()) && _currentElevation != elevation) {
                            menuElevationClicked.emit(elevation);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Run")) {
            // TODO: run in F2 / Falltergeist
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Configuration")) {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

} // namespace geck
