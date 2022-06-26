#pragma once

#include "Panel.h"
#include "editor/Object.h"

#include <optional>

namespace geck {

class SelectedObjectPanel : public Panel {
public:
    SelectedObjectPanel();

    void render(float dt) override;

    void selectObject(std::shared_ptr<Object> selectedObject);

private:
    std::optional<std::shared_ptr<Object>> _selectedObject;
};

} // namespace geck
