#include "SelectedObjectPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

#include "format/map/MapObject.h"
#include "reader/pro/ProReader.h"
#include "ui/util.h"
#include "util/ResourceManager.h"
#include "util/ProHelper.h"

namespace geck {

SelectedObjectPanel::SelectedObjectPanel() {
    _title = "Selected Object";
}

void SelectedObjectPanel::render(float dt) {
    if (!_selectedObject) {
        return;
    }

    ImGui::Begin("Selected object");

    ImGui::Image(_selectedObject.value()->getSprite(), sf::Color::White, sf::Color::Green);

    auto& selected_map_object = _selectedObject.value()->getMapObject();
    int32_t PID = selected_map_object.pro_pid;

    ProReader pro_reader{};
    auto pro = ResourceManager::getInstance().loadResource(ProHelper::basePath(PID), pro_reader);

    auto msg = ProHelper::msgFile(pro->type());
    std::string message = msg->message(pro->header.message_id).text;
    ImGui::InputText("Name", &message, ImGuiInputTextFlags_ReadOnly);

    std::string pro_str = pro->typeToString();
    ImGui::InputText("Type", &pro_str, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Message ID", ImGuiDataType_U32, &pro->header.message_id, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputInt("Position", &selected_map_object.position, 0, 0);
    ImGui::InputScalar("Proto PID", ImGuiDataType_U32, &selected_map_object.pro_pid, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);

    ImGui::End();
}

void SelectedObjectPanel::selectObject(std::shared_ptr<Object> selectedObject) {
    if (selectedObject == nullptr) {
        _selectedObject.reset();
    } else {
        _selectedObject = selectedObject;
    }
}

} // namespace geck
