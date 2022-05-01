#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <filesystem>
#include <thread>

#include "../util/ResourceManager.h"
#include "State.h"

namespace geck {

struct AppData;
class Map;
class Object;

class EditorState : public State {
private:
    EditorState(const std::shared_ptr<AppData>& appData);
    enum class EditorAction {
        NONE,
        PANNING
    };

    void renderMainMenu();
    void centerViewOnMap();

    void loadMapSprites();
    void loadScriptVars();
    void createNewMap();

    std::filesystem::path _dataPath;

    std::vector<sf::Sprite> _floorSprites;
    std::vector<sf::Sprite> _roofSprites;

    std::vector<Object> _objects;

    std::shared_ptr<AppData> _appData;
    sf::View _view;

    int _currentElevation = 0;
    std::unique_ptr<Map> _map;
    std::string _mapScriptName;

    //    std::unordered_map<std::string, uint32_t> _gvars;
    std::unordered_map<std::string, uint32_t> _mvars;

    bool _showObjects = true;
    bool _showCritters = true;
    bool _showRoof = true;
    bool _showWalls = true;
    bool _showScrollBlk = false;

    bool _quit = false;

    sf::Vector2i _lastMousePos{ 0, 0 }; // panning
    EditorAction _currentAction = EditorAction::NONE;

    void showMapInfoPanel();

    void openMap();
    void saveMap();

public:
    EditorState(const std::shared_ptr<AppData>& appData, std::unique_ptr<Map> map);

    void init() override;
    void handleEvent(const sf::Event& event) override;
    void update(const float& dt) override;
    void render(const float& dt) override;

    bool quit() const override;
};

} // namespace geck
