#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "../util/TextureManager.h"
#include "State.h"

#include "../format/map/Map.h"

namespace geck {

class AppData;
class Map;

class EditorState : public State {
private:
    enum class EditorAction { NONE, PANNING };

    void renderMainMenu();
    void centerViewOnMap();
    void loadMap();

    std::vector<sf::Sprite> _floorSprites;
    std::vector<sf::Sprite> _roofSprites;
    std::vector<sf::Sprite> _objectSprites;

    std::shared_ptr<AppData> _appData;
    sf::View _view;

    TextureManager _textureManager;

    int _currentElevation = 0;
    std::unique_ptr<Map> _map;

    bool _showObjects = true;
    bool _showCritters = true;
    bool _showRoof = true;
    bool _showWalls = true;
    bool _showScrollBlk = false;

    bool _quit = false;

    sf::Vector2i _lastMousePos{0, 0};  // panning
    EditorAction _currentAction = EditorAction::NONE;

public:
    EditorState(const std::shared_ptr<AppData>& appData);

    void init() override;
    void handleEvent(const sf::Event& event) override;
    void update(const float& dt) override;
    void render(const float& dt) override;

    bool quit() const override;
};

}  // namespace geck
