#pragma once

#include "Panel.h"
#include "util/Signal.h"

#include <map>
#include <vector>

namespace geck {

class Toolbar : public Panel {
public:

    Toolbar(const std::string& title);

    void render(float dt) override;

    void addButton(const std::string& label, Signal<> callback, const std::string& tooltip = "");

private:

    struct ToolbarButton {
        std::string title;
        std::string tooltip;
        Signal<> callback;
    };

    std::vector<ToolbarButton> _buttons;
};

} // namespace geck
