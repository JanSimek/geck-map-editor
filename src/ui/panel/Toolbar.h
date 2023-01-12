#pragma once

#include "Panel.h"
#include "util/Signal.h"

#include <map>

namespace geck {

class Toolbar : public Panel {
public:

    Toolbar(const std::string& title);

    void render(float dt) override;

    void addButton(const std::string& label, Signal<> callback);

private:

    std::map<std::string, Signal<>> _buttonCallback;
};

} // namespace geck
