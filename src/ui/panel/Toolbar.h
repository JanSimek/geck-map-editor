#pragma once

#include "Panel.h"

#include "imgui_internal.h"
#include "util/Signal.h"

#include <map>

namespace geck {

class Toolbar : public Panel {
public:
    Toolbar(const std::string& title, ImGuiAxis* _axis);

    void render(float dt) override;

    void addButton(const std::string& label, Signal<> callback);

private:
    // ImGuiAxis_X = horizontal toolbar
    // ImGuiAxis_Y = vertical toolbar
    ImGuiAxis* _axis;

    std::map<std::string, Signal<>> _buttonCallback;
};

} // namespace geck
