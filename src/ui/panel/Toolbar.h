#pragma once

#include "Panel.h"
#include "util/Signal.h"

#include <imgui.h>
#include <map>

namespace geck {

/**
 * Slightly worse version of https://github.com/ocornut/imgui/issues/2648 but without the use of imgui_internal.h
 */
class Toolbar : public Panel {
public:

    enum class ORIENTATION
    {
        HORIZONTAL,
        VERTICAL
    };

    Toolbar(const std::string& title, ORIENTATION _axis);

    void render(float dt) override;

    void addButton(const std::string& label, Signal<> callback);

private:
    ORIENTATION _axis;

    std::map<std::string, Signal<>> _buttonCallback;
};

} // namespace geck
