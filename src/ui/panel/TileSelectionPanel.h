#pragma once

#include "Panel.h"

#include "util/Signal.h"
#include <SFML/Graphics/Sprite.hpp>

namespace geck {

class TileSelectionPanel : public Panel {
public:
    TileSelectionPanel();

    void render(float dt) override;

    Signal<int> tileClicked;

private:
    std::vector<sf::Sprite> _selectableTileSprites;
};

} // namespace geck
