#include "Toolbar.h"

#include <spdlog/spdlog.h>
#include <imgui_internal.h>

namespace geck {

Toolbar::Toolbar(const std::string& title) {
    _title = title;
}

void Toolbar::render(float dt) {

    ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    float height = ImGui::GetFrameHeight();

    if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags)) {
        if (ImGui::BeginMenuBar()) {
            for (auto& button : _buttons) {
                if (ImGui::SmallButton(button.labelGetter().c_str())) {
                    button.callback.emit();
                }
                if (ImGui::IsItemHovered() && !button.tooltip.empty()) {
                    ImGui::SetTooltip("%s", button.tooltip.c_str());
                }
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    /*
    if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::Text("Happy status bar");
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }
    */
}

void Toolbar::addButton(std::function<std::string()> labelGetter, Signal<> callback, const std::string& tooltip) {
    _buttons.push_back({ labelGetter, tooltip, std::move(callback) });
    //_buttonCallback[label] = std::move(callback);
}

} // namespace geck
