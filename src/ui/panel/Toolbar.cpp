#include "Toolbar.h"

#include <spdlog/spdlog.h>

namespace geck {

Toolbar::Toolbar(const std::string& title, ORIENTATION toolbarAxis)
    : _axis(toolbarAxis) {
    _title = title;
}

void Toolbar::render(float dt) {
    // 1. We request auto-sizing on one axis
    // Note however this will only affect the toolbar when NOT docked.
    ImVec2 requested_size = (_axis == ORIENTATION::HORIZONTAL) ? ImVec2(-1.0f, 0.0f) : ImVec2(0.0f, -1.0f);
    ImGui::SetNextWindowSize(requested_size);

    // 2. Specific docking options for toolbars.
    ImGuiWindowClass window_class;
    window_class.DockingAllowUnclassed = true;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_AutoHideTabBar;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingInCentralNode;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;
    ImGui::SetNextWindowClass(&window_class);

    // 3. Begin into the window
    const float font_size = ImGui::GetFontSize();
    const ImVec2 icon_size(font_size * 1.7f, font_size * 1.7f);
    ImGui::Begin(_title.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    // Populate tab bar
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));
    // UndockWidget(icon_size, toolbar_axis);

    for (auto& button : _buttonCallback) {
        if (ImGui::Button(button.first.c_str(), icon_size)) {
            button.second.emit();
        }
        if (_axis == ORIENTATION::HORIZONTAL)
            ImGui::SameLine();
    }
    ImGui::PopStyleVar(2);

    ImGui::End();
}

void Toolbar::addButton(const std::string& label, Signal<> callback) {
    _buttonCallback[label] = std::move(callback);
}

} // namespace geck
