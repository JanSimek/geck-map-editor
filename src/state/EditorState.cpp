#include "EditorState.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <cmath> // ceil
#include <portable-file-dialogs.h>

#include "../ui/util.h"

#include "../editor/Object.h"

#include "../reader/dat/DatReader.h"
#include "../reader/frm/FrmReader.h"
#include "../reader/lst/LstReader.h"
#include "../reader/gam/GamReader.h"
#include "../reader/pro/ProReader.h"

#include "../writer/map/MapWriter.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/frm/Frm.h"
#include "../format/lst/Lst.h"
#include "../format/gam/Gam.h"
#include "../format/map/Tile.h"
#include "../format/msg/Msg.h"
#include "../format/pro/Pro.h"
#include "../format/map/MapObject.h"

#include "loader/MapLoader.h"

#include "../util/FileHelper.h"
#include "../util/ProHelper.h"

#include "LoadingState.h"
#include "StateMachine.h"
#include "reader/msg/MsgReader.h"

namespace geck {

EditorState::EditorState(const std::shared_ptr<AppData>& appData, std::unique_ptr<Map> map)
    : _appData(appData)
    , _view({ 0.f, 0.f }, sf::Vector2f(appData->window->getSize()))
    , _dataPath(FileHelper::getInstance().fallout2DataPath())
    , _map(std::move(map)) {
    centerViewOnMap();
}

void EditorState::init() {
    std::filesystem::path map_directory = _dataPath / "maps";

    if (!std::filesystem::is_directory(map_directory)) {
        pfd::message("Invalid Fallout 2 directory",
            "The map directory does not exist: " + map_directory.string(),
            pfd::choice::ok,
            pfd::icon::error);
        _quit = true;
        return;
    }

    loadSprites();

    loadScriptVars();
}

void EditorState::saveMap() {
    MapWriter map_writer{ _dataPath, [](int32_t PID) {
                             ProReader pro_reader{};
                             return ResourceManager::getInstance().loadResource(ProHelper::basePath(PID), pro_reader);
                         } };
    // TODO: show file dialog
    map_writer.openFile("test.map");
    if (map_writer.write(_map->getMapFile())) {
        spdlog::info("Saved map test.map");
    } else {
        spdlog::error("Failed to save map test.map");
    }
}

void EditorState::openMap() {
    auto loading_state = std::make_unique<LoadingState>(_appData);
    loading_state->addLoader(std::make_unique<MapLoader>("", -1, [&](auto map) {
        _appData->stateMachine->push(std::make_unique<EditorState>(_appData, std::move(map)), true);
    }));

    _appData->stateMachine->push(std::move(loading_state));
}

void EditorState::renderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE " New map", "Ctrl+N")) {
                createNewMap();
            }
            if (ImGui::MenuItem(ICON_FA_SAVE " Save map", "Ctrl+S")) {
                saveMap();
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open map", "Ctrl+O")) {
                openMap();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                _quit = true;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            bool disabled = true;
            ImGui::MenuItemCheckbox("Show objects", &_showObjects);
            ImGui::MenuItemCheckbox("Show critters", &_showCritters, disabled);
            ImGui::MenuItemCheckbox("Show walls", &_showWalls, disabled);
            ImGui::MenuItemCheckbox("Show roofs", &_showRoof);
            ImGui::MenuItemCheckbox("Show scroll block", &_showScrollBlk);

            if (_map) {
                ImGui::Separator();
                ImGui::Tooltip("Currently visible map elevation");
                ImGui::SameLine();
                if (ImGui::BeginMenu("Elevation")) {
                    if (_map->elevations() == 1) {
                        ImGui::Text("Map has only 1 elevation");
                    } else {
                        for (int i = 0; i < _map->elevations(); i++) {
                            std::string text = std::to_string(i) + (i == _currentElevation ? " " ICON_FA_CHECK_CIRCLE : "");
                            if (ImGui::MenuItem(text.c_str()) && _currentElevation != i) {
                                _currentElevation = i;
                                spdlog::info("Loading elevation " + std::to_string(_currentElevation));

                                auto loading_state = std::make_unique<LoadingState>(_appData);
                                loading_state->addLoader(std::make_unique<MapLoader>(_map->path(), _currentElevation, [&](std::unique_ptr<Map> map) {
                                    _map = std::move(map);
                                    init();
                                    _appData->stateMachine->pop();
                                }));

                                _appData->stateMachine->push(std::move(loading_state));
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Run")) {
            // TODO: run in F2 / Falltergeist
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Configuration")) {

            ImGui::OpenPopup("Config");
            ImGui::EndMenu();
        }

        auto fpsTextSize = ImGui::CalcTextSize("%.2f FPS");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fpsTextSize.x - ImGui::GetStyle().ItemSpacing.x * 2.0f);

        ImGui::Text("%.2f FPS", _fps);

        ImGui::EndMainMenuBar();
    }
}

void EditorState::loadObjectSprites() {
    // Objects
    if (_map->objects().empty())
        return;

    for (const auto& object : _map->objects().at(_currentElevation)) {
        if (object->position == -1)
            continue; // object inside an inventory/container

        const std::string frm_name = ResourceManager::getInstance().FIDtoFrmName(object->frm_pid);

        spdlog::debug("Loading sprite {}", frm_name);

        const auto& frm = ResourceManager::getInstance().getResource<Frm>(frm_name);

        // TODO: use _objects.insert(v.begin(), Object{ frm }); for flat objects
        //       which should be rendered first

        _objects.emplace_back(std::make_shared<Object>(frm));
        sf::Sprite object_sprite{ ResourceManager::getInstance().texture(frm_name) };
        _objects.back()->setSprite(std::move(object_sprite));
        _objects.back()->setHexPosition(_hexgrid.grid().at(object->position));
        _objects.back()->setMapObject(object);
        _objects.back()->setDirection(object->direction);
    }
}

// Tiles
// TODO: create tile atlas like in Falltergeist Tilemap.cpp
void EditorState::loadTileSprites() {
    const auto& lst = ResourceManager::getInstance().getResource<Lst, std::string>("art/tiles/tiles.lst");

    for (auto tileNumber = 0U; tileNumber < geck::Map::TILES_PER_ELEVATION; ++tileNumber) {
        auto tile = _map->getMapFile().tiles.at(_currentElevation).at(tileNumber);

        // Positioning

        // geometry constants
        // const TILE_WIDTH = 80
        // const TILE_HEIGHT = 36
        // const HEX_GRID_SIZE = 200 // hex grid is 200x200 (roof+floor)

        unsigned int tileX = static_cast<unsigned>(ceil(((double)tileNumber) / 100));
        unsigned int tileY = tileNumber % 100;
        unsigned int x = (100 - tileY - 1) * 48 + 32 * (tileX - 1);
        unsigned int y = tileX * 24 + (tileY - 1) * 12 + 1;

        const auto& createTileSprite = [&](const uint16_t tile_id, int offset = 0) {
            sf::Sprite tile_sprite;
            std::string texture_path = "art/tiles/" + lst->at(tile_id);
            tile_sprite.setTexture(ResourceManager::getInstance().texture(texture_path));
            tile_sprite.setPosition(x, y - offset);
            return tile_sprite;
        };

        // floor
        uint16_t floorId = tile.getFloor();
        if (floorId == Map::EMPTY_TILE) {
            sf::Sprite tile_sprite;
            tile_sprite.setTexture(ResourceManager::getInstance().texture("art/tiles/blank.frm"));
            tile_sprite.setPosition(x, y);
            _floorSprites[tileNumber] = tile_sprite;
        } else {
            _floorSprites[tileNumber] = createTileSprite(floorId);
        }

        // roof
        constexpr int roofOffset = 96; // "roof height"
        _roofSprites[tileNumber] = createTileSprite(tile.getRoof(), roofOffset);
    }

    // selectable tiles
    for (int tile_id = 0; tile_id < lst->list().size(); tile_id++) {
        sf::Sprite tile_sprite;
        std::string texture_path = "art/tiles/" + lst->at(tile_id);
        tile_sprite.setTexture(ResourceManager::getInstance().texture(texture_path));
        _selectableTileSprites.push_back(tile_sprite);
    }
}

void EditorState::loadSprites() {

    spdlog::stopwatch sw;

    _appData->window->setTitle(_map->filename() + " - GECK::Mapper");

    _objects.clear();
    _floorSprites = {};
    _roofSprites = {};

    // Data

    loadTileSprites();

    loadObjectSprites();

    spdlog::info("Map sprites loaded in {:.3} seconds", sw);
}

void EditorState::loadScriptVars() {

    // Global variables
    GamReader gam_reader{};
    auto gam_filename = _map->filename().substr(0, _map->filename().find(".")) + ".gam";
    auto gam_file = gam_reader.openFile(_dataPath / "maps" / gam_filename);

    for (int index = 0; index < _map->getMapFile().header.num_global_vars; index++) {
        _mvars.emplace(gam_file->mvarKey(index), gam_file->mvarValue(index));
    }

    int map_script_id = _map->getMapFile().header.script_id;
    if (map_script_id > 0) {
        LstReader lst_reader{};
        auto scripts = lst_reader.openFile(_dataPath / "scripts" / "scripts.lst", std::ifstream::in);
        _mapScriptName = scripts->at(map_script_id - 1); // script id starts at 1
    }
}

std::vector<bool> EditorState::calculateBitset(const sf::Image& img) {
    sf::Vector2u imgSize = img.getSize();

    std::vector<bool> retVal(imgSize.x * imgSize.y); // allocate directly

    for (unsigned int x = 0; x < imgSize.x; ++x)     // unsigned,  pre-increment (not crucial
        for (unsigned int y = 0; y < imgSize.y; ++y) // here, but generally good practice)
        {
            const auto& pixel = img.getPixel(x, y);
            retVal[imgSize.x * y + x] = (pixel.r == 0 && pixel.g == 0 && pixel.b == 0 && pixel.a == 0);
        }

    return retVal;
}

bool EditorState::selectObject(sf::Vector2f worldPos) {

    // TODO: use ranges ?
    // filter and then max_element using the position property
    std::optional<std::shared_ptr<Object>> newly_selected_object;

    // we are moving selected object to a different location
    if (_selectedObject && !isSpriteClicked(worldPos, _selectedObject->get()->getSprite())) {
        return false;
    }

    // we are clicking on the same area where the selected object is located, so select the one behind or unselect it
    for (int i = 0; i < _objects.size(); i++) {
        auto& iterated_object = _objects.at(i);

        if (isSpriteClicked(worldPos, iterated_object->getSprite())) {

            if (iterated_object->isSelected()) {
                unselectObject();
                break;
            }

            if (newly_selected_object) {

                const int position1 = newly_selected_object->get()->getMapObject().position;
                const int position2 = iterated_object->getMapObject().position;

                // select the foremost object
                if (position1 < position2) {
                    newly_selected_object = iterated_object;
                }
            } else {
                newly_selected_object = iterated_object;
            }
        }
    }
    if (newly_selected_object) {

        unselectAll();

        _selectedObject = newly_selected_object;
        _selectedObject->get()->select();

        return true;
    }

    return false;
}

// TODO: select roof tiles
bool EditorState::selectTile(sf::Vector2f worldPos) {
    for (int i = 0; i < _floorSprites.size(); i++) {
        auto& tile = _floorSprites.at(i);
        // mouse is within the tile rect
        if (isSpriteClicked(worldPos, tile)) {
            if (std::count(_selectedTileIndexes.begin(), _selectedTileIndexes.end(), i)) {
                tile.setColor(sf::Color::White);
                _selectedTileIndexes.erase(std::remove(_selectedTileIndexes.begin(), _selectedTileIndexes.end(), i));
                return false;
            } else {
                tile.setColor(sf::Color::Red);
                _selectedTileIndexes.push_back(i);

                unselectObject();
                return true;
            }
        }
    }

    return false;
}

bool EditorState::isSpriteClicked(const sf::Vector2f& worldPos, const sf::Sprite& sprite) {
    if (sprite.getGlobalBounds().contains(worldPos)) {

        const auto& image = sprite.getTexture()->copyToImage();
        const auto& bitsets = calculateBitset(image);

        const auto& x = worldPos.x - sprite.getGlobalBounds().left;
        const auto& y = worldPos.y - sprite.getGlobalBounds().top;

        int w = image.getSize().x;
        if (!bitsets[w * y + x]) {
            return true;
        }
    }
    return false;
}

void EditorState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::N: // Ctrl+N
                if (event.key.control)
                    createNewMap();
                break;
            case sf::Keyboard::Q: // Ctrl+Q
                if (event.key.control)
                    _quit = true;
                break;
            case sf::Keyboard::S: // Ctrl+S
                if (event.key.control)
                    saveMap();
                break;
            case sf::Keyboard::O: // Ctrl+O
                if (event.key.control)
                    openMap();
                break;
            case sf::Keyboard::R: // R
                if (_selectedObject) {
                    _selectedObject->get()->rotate();
                }
                break;
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
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel) {
        float delta = event.mouseWheelScroll.delta;
        _view.zoom(1.0f - delta * 0.05f);
    }

    // Left mouse click
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {

        sf::Vector2i mousePos = sf::Mouse::getPosition(*_appData->window);

        // convert it to world coordinates
        sf::Vector2f worldPos = _appData->window->mapPixelToCoords(mousePos);

        bool object_already_selected = _selectedObject.has_value(); // some object was already selected
        bool selected_different_object = selectObject(worldPos);    // another object was selected instead

        // still have the original selected and clicked on an empty space
        bool moving_object = object_already_selected && !selected_different_object && _selectedObject.has_value();

        auto position_clicked = _hexgrid.positionAt(worldPos.x, worldPos.y);
        if (moving_object && position_clicked != Hex::HEX_OUT_OF_MAP) {

            _selectedObject->get()->setHexPosition(_hexgrid.grid().at(position_clicked));

            std::stable_sort(_objects.begin(), _objects.end(), [](const auto obj1, const auto obj2) {
                return obj1->getMapObject().position < obj2->getMapObject().position;
            });
        }

        if (!_selectedObject && !moving_object && !object_already_selected) {
            selectTile(worldPos);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right) {

        // Initialize panning
        _mouseStartingPosition = sf::Mouse::getPosition(*_appData->window);
        _mouseLastPosition = _mouseStartingPosition;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {

        if (_currentAction == EditorAction::PANNING) {

            sf::Vector2f mousePositionDiff = sf::Vector2f{ sf::Mouse::getPosition(*_appData->window) - _mouseLastPosition };
            _view.move(-1.0f * mousePositionDiff);

        } else {
            constexpr int panningThreshold = 5;

            int mouseMovedX = std::abs(_mouseStartingPosition.x - _mouseLastPosition.x);
            int mouseMovedY = std::abs(_mouseStartingPosition.y - _mouseLastPosition.y);

            if (mouseMovedX > panningThreshold || mouseMovedY > panningThreshold) {
                _currentAction = EditorAction::PANNING;
                if (_cursor.loadFromSystem(sf::Cursor::SizeAll)) {
                    _appData->window->setMouseCursor(_cursor);
                }
            }
        }
        _mouseLastPosition = sf::Mouse::getPosition(*_appData->window);
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {

        if (_currentAction != EditorAction::PANNING) {
            unselectAll();
        }

        _currentAction = EditorAction::NONE;

        if (_cursor.loadFromSystem(sf::Cursor::Arrow)) {
            _appData->window->setMouseCursor(_cursor);
        }
    }

    // Window resizing
    if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
        _view.reset(visibleArea);
        centerViewOnMap();
    }
}

void EditorState::update(const float dt) { }

/**
 * @brief UI widget for displaying properties from the current MAP file
 */
void EditorState::showMapInfoPanel() {
    auto mapInfo = _map->getMapFile();
    int elevations = mapInfo.tiles.size();

    ImGui::Begin(ICON_FA_MAP_MARKER_ALT " Map Information");

    const auto addInputScalar = [](const auto& label, const auto& value) {
        ImGui::InputScalar(label, ImGuiDataType_U32, value, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
    };

    constexpr float left_column_width = 0.35f;

    if (ImGui::CollapsingHeader("Map header")) {

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * left_column_width);
        ImGui::LabelText("Property", "Value");

        ImGui::InputText("Filename", &mapInfo.header.filename, ImGuiInputTextFlags_ReadOnly);
        addInputScalar("Map elevations", &elevations);
        addInputScalar("Player default position", &mapInfo.header.player_default_position);
        addInputScalar("Player default elevation", &mapInfo.header.player_default_elevation);
        addInputScalar("Player default orientation", &mapInfo.header.player_default_orientation);
        addInputScalar("Global variables #", &mapInfo.header.num_global_vars);
        addInputScalar("Local variables #", &mapInfo.header.num_local_vars);
        ImGui::InputText("Map script", &_mapScriptName, ImGuiInputTextFlags_ReadOnly);
        addInputScalar("Map script ID", &mapInfo.header.script_id);
        addInputScalar("Darkness", &mapInfo.header.darkness);
        addInputScalar("Map ID", &mapInfo.header.map_id);
        addInputScalar("Timestamp", &mapInfo.header.timestamp);
        bool isSavegame = ((mapInfo.header.flags & 0x1) != 0);
        ImGui::Checkbox("Save game map", &isSavegame);
    }

    if (ImGui::CollapsingHeader("Map global variables")) {

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * left_column_width);
        ImGui::LabelText("Variable", "Value");

        for (auto& [key, value] : _mvars) {
            addInputScalar(key.c_str(), &value);
        }
    }

    if (ImGui::CollapsingHeader("Map scripts")) {
        // TODO
    }

    ImGui::End(); // Map Information
}

void EditorState::showTilesPanel() {
    ImGui::Begin(ICON_FA_MAP " Tiles");

    float content_width = ImGui::GetContentRegionAvail().x;
    const float item_width = Tile::TILE_WIDTH;
    int columns = content_width / item_width;
    if (columns == 0) {
        columns = 1;
    }
    ImGui::Columns(columns, nullptr, false);

    for (int i = 0; const auto& tile : _selectableTileSprites) {

        // hide reserved.frm and grid000.frm
        if (i < 2) {
            ++i;
            continue;
        }

        if (ImGui::ImageButton(tile)) {
            for (auto selectedTileId : _selectedTileIndexes) {
                const auto& lst = ResourceManager::getInstance().getResource<Lst, std::string>("art/tiles/tiles.lst");

                _map->getMapFile().tiles.at(_currentElevation).at(selectedTileId).setFloor(i);

                sf::Sprite tile_sprite;
                std::string texture_path = "art/tiles/" + lst->at(i);
                auto selectedTile = _floorSprites.at(selectedTileId);
                tile_sprite.setTexture(ResourceManager::getInstance().texture(texture_path));
                tile_sprite.setPosition(selectedTile.getPosition().x, selectedTile.getPosition().y);
                _floorSprites.at(selectedTileId) = tile_sprite;
            }
        }
        ImGui::NextColumn();
        ++i;
    }

    ImGui::End();
}

/**
 * TODO: draw only sprites visible in the current view
 */
void EditorState::showSelectedObjPanel() {
    ImGui::Begin("Selected object");

    ImGui::Image(_selectedObject->get()->getSprite(), sf::Color::White, sf::Color::Green);

    auto& selected_map_object = _selectedObject->get()->getMapObject();
    int32_t PID = selected_map_object.pro_pid;

    ProReader pro_reader{};
    auto pro = ResourceManager::getInstance().loadResource(ProHelper::basePath(PID), pro_reader);

    auto msg = ProHelper::msgFile(pro->type());
    std::string message = msg->message(pro->header.message_id).text;
    ImGui::InputText("Name", &message, ImGuiInputTextFlags_ReadOnly);

    std::string pro_str = pro->typeToString();
    ImGui::InputText("Type", &pro_str, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Message ID", ImGuiDataType_U32, &pro->header.message_id, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputInt("Position", &selected_map_object.position, 0, 0);
    ImGui::InputScalar("Proto PID", ImGuiDataType_U32, &selected_map_object.pro_pid, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);

    ImGui::End();
}

void EditorState::unselectAll() {
    unselectObject();
    unselectTiles();
}

void EditorState::unselectTiles() {
    // clear selected tiles
    for (int tile_index : _selectedTileIndexes) {
        _floorSprites.at(tile_index).setColor(sf::Color::White);
    }
    _selectedTileIndexes.clear();
}

void EditorState::unselectObject() {
    if (_selectedObject) {
        _selectedObject->get()->unselect();
        _selectedObject = {};
    }
}

void EditorState::render(const float dt) {

    renderMainMenu();

    _appData->window->setView(_view);

    for (const auto& floor : _floorSprites) {
        _appData->window->draw(floor);
    }

    if (_showObjects) {
        for (const auto& object : _objects) {
            //            if (!_showScrollBlk && object->getMapObject().isBlocker()) {
            //                continue;
            //            }
            _appData->window->draw(object->getSprite());
        }
    }

    if (_showRoof) {
        for (const auto& roof : _roofSprites) {
            _appData->window->draw(roof);
        }
    }

    showTilesPanel();

    showMapInfoPanel();

    if (_selectedObject) {
        showSelectedObjPanel(); // Selected object
    }

    if (ImGui::BeginPopup("Config")) {
        ImGui::Text("Hello world");
    }

    _fps = 1.f / dt;
}

void EditorState::centerViewOnMap() {
    constexpr float center_x = Tile::TILE_WIDTH * 50;
    constexpr float center_y = Tile::TILE_HEIGHT * 50;
    _view.setCenter(center_x, center_y);
}

bool EditorState::quit() const {
    return _quit;
}

void EditorState::createNewMap() {
    int elevations = 1;
    int floorTileIndex = 192; // edg5000.frm
    int roofTileIndex = 1;    // grid000.frm

    _map = std::make_unique<Map>("test.map");
    auto map_file = std::make_unique<Map::MapFile>();
    _map->setMapFile(std::move(map_file));

    LstReader lst_reader;
    std::filesystem::path data_path = FileHelper::getInstance().fallout2DataPath();
    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    std::string texture_path = "art/tiles/" + lst->at(floorTileIndex);
    ResourceManager::getInstance().insertTexture(texture_path);

    texture_path = "art/tiles/" + lst->at(roofTileIndex);
    ResourceManager::getInstance().insertTexture(texture_path);

    std::map<int, std::vector<Tile>> tiles;
    for (auto elevation = 0; elevation < elevations; ++elevation) {
        for (auto i = 0U; i < Map::TILES_PER_ELEVATION; ++i) {
            uint16_t roof = roofTileIndex;
            uint16_t floor = floorTileIndex;

            Tile tile(floor, roof);
            tiles[elevation].push_back(tile);
        }
    }
    _map->getMapFile().tiles = tiles;

    loadSprites();
}

} // namespace geck
