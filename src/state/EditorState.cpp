#include "EditorState.h"

#include <imgui.h>
#include <spdlog/spdlog.h>
#include <cmath>  // ceil
#include <fstream>

#include "../Application.h"
#include "../ui/IconsFontAwesome5.h"
#include "../ui/util.h"
#include "../util/TextureManager.h"

#include "../editor/HexagonGrid.h"
#include "../editor/MapObject.h"
#include "../editor/Tile.h"
#include "StateMachine.h"

#include "../reader/dat/DatReader.h"
#include "../reader/frm/FrmReader.h"
#include "../reader/lst/LstReader.h"
#include "../reader/map/MapReader.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/frm/Frm.h"
#include "../format/lst/Lst.h"
#include "../format/map/Map.h"
#include "../format/pal/Pal.h"

#include "../util/FileHelper.h"

namespace geck {

EditorState::EditorState(const std::shared_ptr<AppData>& appData)
    : _appData(appData), _view({0.f, 0.f}, sf::Vector2f(appData->window->getSize())) {
    _textureManager.setDataPath(FileHelper::getInstance().path());
    centerViewOnMap();
}

void EditorState::init() {
    // FIXME:   Figure out how to load textures in another thread.
    //          sf::Texture cannot be created outside of the main thread
    //          https://www.sfml-dev.org/tutorials/2.5/window-opengl.php#rendering-from-threads
    spdlog::warn("FIXME: Initializing map. The application will hang for a few seconds");
    loadMap();
}

void EditorState::renderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(ICON_FA_FILE_ALT " File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                _quit = true;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_EYE " View")) {
            bool disabled = true;
            ImGui::MenuItemCheckbox("Show objects", &_showObjects);
            ImGui::MenuItemCheckbox("Show critters", &_showCritters, disabled);
            ImGui::MenuItemCheckbox("Show walls", &_showWalls, disabled);
            ImGui::MenuItemCheckbox("Show roofs", &_showRoof);
            ImGui::MenuItemCheckbox("Show scroll block", &_showScrollBlk, disabled);

            if (_map) {
                ImGui::Separator();
                ImGui::Tooltip("Currently visible map elevation");
                ImGui::SameLine();
                if (ImGui::BeginMenu("Elevation")) {
                    if (_map->elevations() == 1) {
                        ImGui::Text("Map has only 1 elevation");
                    } else {
                        for (int i = 0; i < _map->elevations(); i++) {
                            std::string text =
                                std::to_string(i) + (i == _currentElevation ? " " ICON_FA_CHECK_CIRCLE : "");
                            if (ImGui::MenuItem(text.c_str()) && _currentElevation != i) {
                                _currentElevation = i;
                                spdlog::info("Loading elevation " + std::to_string(_currentElevation));
                                loadMap();
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void geck::EditorState::loadMap() {
    const std::string data_path = FileHelper::getInstance().path();

    _floorSprites.clear();
    _roofSprites.clear();
    _objectSprites.clear();

    // Data

    MapReader map_reader;
    auto map = map_reader.read(data_path + "maps/" + _appData->mapName);

    LstReader lst_reader;
    auto lst = lst_reader.read(data_path + "art/tiles/tiles.lst");

    for (auto i = 0U; i < geck::Map::ROWS * geck::Map::COLS; ++i) {
        auto tile = map->tiles().at(_currentElevation).at(i);

        // floor
        sf::Sprite floor_sprite;
        std::string floor_texture_path = "art/tiles/" + lst->at(tile.getFloor());
        _textureManager.insert(floor_texture_path);
        floor_sprite.setTexture(_textureManager.get(floor_texture_path));

        // roof
        sf::Sprite roof_sprite;
        std::string roof_texture_path = "art/tiles/" + lst->at(tile.getRoof());
        _textureManager.insert(roof_texture_path);
        roof_sprite.setTexture(_textureManager.get(roof_texture_path));

        // Positioning

        unsigned int tileX = static_cast<unsigned>(ceil(((double)i) / 100));
        unsigned int tileY = i % 100;
        unsigned int x = (100 - tileY - 1) * 48 + 32 * (tileX - 1);
        unsigned int y = tileX * 24 + (tileY - 1) * 12 + 1;

        // floor
        floor_sprite.setPosition(x, y);

        // roof
        constexpr int roofOffset = 96;
        roof_sprite.setPosition(x, y - roofOffset);

        _floorSprites.push_back(std::move(floor_sprite));
        _roofSprites.push_back(std::move(roof_sprite));
    }

    auto hexgrid = geck::HexagonGrid();

    FrmReader frm_reader;

    for (const auto& object : map->objects().at(_currentElevation)) {
        if (object->hexPosition() == -1)
            continue;  // object inside an inventory/container

        if (object->frm().empty()) {
            spdlog::error("Empty FRM file path on hex number " + std::to_string(object->hexPosition()));
            continue;  // this should probably never happen
        }

        sf::Sprite object_sprite;
        _textureManager.insert(object->frm());
        object_sprite.setTexture(_textureManager.get(object->frm()));

        //        int hexPos = object->hexPosition();
        //        float x = hexPos % 200;
        //        float y = hexPos / 200;

        //        Point point = hexToScreen(x, y);

        const geck::Hex* hex = hexgrid.grid().at(object->hexPosition()).get();

        auto frm = frm_reader.read(data_path + object->frm());

        // center on the hex
        object_sprite.setPosition(
            // X
            (float)hex->x() + frm->directions().front().shiftX() - (object_sprite.getTexture()->getSize().x / 2),

            // Y
            (float)hex->y() + frm->directions().front().shiftY() - object_sprite.getTexture()->getSize().y);

        _objectSprites.push_back(std::move(object_sprite));
    }

    _map = std::move(map);
    spdlog::info("Map loaded");
}

void EditorState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Q:  // Ctrl+Q
                if (event.key.control)
                    _quit = true;
            case sf::Keyboard::Escape:
                _quit = true;
                break;
            case sf::Keyboard::Left:
                _view.move(-50.f, 0.f);
                break;
            case sf::Keyboard::Right:
                _view.move(50.f, 0.f);
                break;
            case sf::Keyboard::Up:
                _view.move(0.f, -50.f);
                break;
            case sf::Keyboard::Down:
                _view.move(0.f, 50.f);
                break;
            default:
                break;
        }
    }

    // Zoom
    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel) {
        float delta = event.mouseWheelScroll.delta;
        _view.zoom(1.0f - delta * 0.1f);
    }

    // Panning
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right) {
        _currentAction = EditorAction::PANNING;
        _lastMousePos = sf::Mouse::getPosition(*_appData->window);
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
        _currentAction = EditorAction::NONE;
    }
    if (_currentAction == EditorAction::PANNING) {
        sf::Vector2f mousePos = sf::Vector2f{sf::Mouse::getPosition(*_appData->window) - _lastMousePos};
        _view.move(-1.0f * mousePos);
        _lastMousePos = sf::Mouse::getPosition(*_appData->window);
    }

    // Window resizing
    if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
        _view.reset(visibleArea);
        centerViewOnMap();
    }
}

void EditorState::update(const float& dt) {}

void EditorState::render(const float& dt) {
    renderMainMenu();

    _appData->window->setView(_view);

    for (const auto& sprite : _floorSprites) {
        _appData->window->draw(sprite);
    }

    if (_showObjects) {
        for (const auto& sprite : _objectSprites) {
            _appData->window->draw(sprite);
        }
    }

    if (_showRoof) {
        for (const auto& sprite : _roofSprites) {
            _appData->window->draw(sprite);
        }
    }

    //    auto boldFont = io.Fonts->Fonts[0];
    //    ImGui::PushFont(0);

    //    ImGui::Begin("Torr Buckner");
    //    ImGui::Text(ICON_FA_FILE_IMAGE " NMWARRAA.FRM" );

    //    ImGui::End();
    //    ImGui::PopFont();
}

void EditorState::centerViewOnMap() {
    float center_x = Tile::TILE_WIDTH * 50;
    float center_y = Tile::TILE_HEIGHT * 50;
    _view.setCenter(center_x, center_y);
}

bool EditorState::quit() const {
    return _quit;
}

}  // namespace geck
