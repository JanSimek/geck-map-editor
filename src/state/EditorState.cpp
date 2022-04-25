#include "EditorState.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <cmath> // ceil
#include <portable-file-dialogs.h>

#include "../ui/util.h"

#include "../editor/HexagonGrid.h"
#include "../editor/Object.h"

#include "../reader/dat/DatReader.h"
#include "../reader/frm/FrmReader.h"
#include "../reader/lst/LstReader.h"
#include "../reader/map/MapReader.h"

#include "../writer/map/MapWriter.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/frm/Frm.h"
#include "../format/lst/Lst.h"

#include "../util/FileHelper.h"
#include "LoadingState.h"

namespace geck {

EditorState::EditorState(const std::shared_ptr<AppData>& appData, std::unique_ptr<Map> map)
    : _appData(appData)
    , _view({ 0.f, 0.f }, sf::Vector2f(appData->window->getSize())) {
    _map = std::move(map);
    centerViewOnMap();
}

void EditorState::init() {
    std::filesystem::path data_path = FileHelper::getInstance().fallout2DataPath();
    std::filesystem::path map_directory = data_path / "maps";

    if (!std::filesystem::is_directory(map_directory)) {
        pfd::message("Invalid Fallout 2 directory",
            "The map directory does not exist: " + map_directory.string(),
            pfd::choice::ok,
            pfd::icon::error);
        _quit = true;
        return;
    }

    loadMap();
}

void EditorState::renderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(ICON_FA_FILE_ALT " File")) {
            if (ImGui::MenuItem(ICON_FA_FILE " New map", "Ctrl+N")) {
                createNewMap();
            }
            if (ImGui::MenuItem(ICON_FA_SAVE " Save map", "Ctrl+S")) {
                MapWriter map_writer;
                // TODO: show file dialog
                map_writer.openFile("test.map");
                map_writer.write(_map->getMapFile());
                spdlog::info("Saved map test.map");
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Load map", "Ctrl+L")) {
                _appData->mapPath = "";
                auto loading_state = std::make_unique<LoadingState>(_appData);
                loading_state->addLoader(std::make_unique<MapLoader>(_appData->mapPath, 0)); // FIXME

                _appData->stateMachine->push(std::move(loading_state));
            }
            ImGui::Separator();
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
                            std::string text = std::to_string(i) + (i == _currentElevation ? " " ICON_FA_CHECK_CIRCLE : "");
                            if (ImGui::MenuItem(text.c_str()) && _currentElevation != i) {
                                _currentElevation = i;
                                spdlog::info("Loading elevation " + std::to_string(_currentElevation));
                                init(); // reload map
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_PLAY_CIRCLE " Run")) {
            // TODO: run in F2 / Falltergeist
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void geck::EditorState::loadMap() {

    spdlog::stopwatch sw;

    const auto data_path = FileHelper::getInstance().fallout2DataPath();

    _appData->window->setTitle(_appData->mapPath.string() + " - GECK::Mapper");

    _objects.clear();
    _floorSprites.clear();
    _roofSprites.clear();

    // Data

    MapReader map_reader;

    LstReader lst_reader;
    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    // Tiles
    // TODO: create tile atlas like in Falltergeist Tilemap.cpp
    for (auto tileNumber = 0U; tileNumber < geck::Map::TILES_PER_ELEVATION; ++tileNumber) {
        auto tile = _map->tiles().at(_currentElevation).at(tileNumber);

        // Positioning

        // geometry constants
        //        const TILE_WIDTH = 80
        //        const TILE_HEIGHT = 36
        //        const HEX_GRID_SIZE = 200 // hex grid is 200x200

        unsigned int tileX = static_cast<unsigned>(ceil(((double)tileNumber) / 100));
        unsigned int tileY = tileNumber % 100;
        unsigned int x = (100 - tileY - 1) * 48 + 32 * (tileX - 1);
        unsigned int y = tileX * 24 + (tileY - 1) * 12 + 1;

        // floor
        if (tile.getFloor() != Map::EMPTY_TILE) {
            sf::Sprite floor_sprite;
            std::string floor_texture_path = "art/tiles/" + lst->at(tile.getFloor());
            floor_sprite.setTexture(ResourceManager::getInstance().texture(floor_texture_path));
            floor_sprite.setPosition(x, y);
            _floorSprites.push_back(std::move(floor_sprite));
        }

        // roof
        if (tile.getRoof() != Map::EMPTY_TILE) {
            sf::Sprite roof_sprite;
            std::string roof_texture_path = "art/tiles/" + lst->at(tile.getRoof());
            roof_sprite.setTexture(ResourceManager::getInstance().texture(roof_texture_path));

            // FIXME: delete - probably won't be needed after reading the correct color.pal
            //            if (tile.getRoof() == 0 || tile.getRoof() == 1) {
            //                roof_sprite.setColor(sf::Color{0, 0, 0, 0});
            //            }
            constexpr int roofOffset = 96; // "roof height"
            roof_sprite.setPosition(x, y - roofOffset);

            _roofSprites.push_back(std::move(roof_sprite));
        }
    }

    auto hexgrid = geck::HexagonGrid();

    // Objects
    if (_map->objects().empty())
        return;

    for (const auto& object : _map->objects().at(_currentElevation)) {
        if (object->position == -1)
            continue; // object inside an inventory/container

        const std::string frmName = map_reader.FIDtoFrmName(object->frm_pid);

        if (frmName.empty()) {
            spdlog::error("Empty FRM file path on hex number " + std::to_string(object->position));
            continue; // this should probably never happen
        }

        sf::Sprite object_sprite;
        object_sprite.setTexture(ResourceManager::getInstance().texture(frmName));

        //        int hexPos = object->hexPosition();
        //        float x = hexPos % 200;
        //        float y = hexPos / 200;

        //        Point point = hexToScreen(x, y);

        const geck::Hex* hex = hexgrid.grid().at(object->position).get();

        const auto& frm = ResourceManager::getInstance().get<Frm>(frmName);

        spdlog::info("Loading sprite {}", frmName);

        // center on the hex
        auto direction_index = object->direction;

        // FIXME: ??? one scrblk on arcaves.map
        if (frm->directions().size() <= direction_index) {
            spdlog::error("Object has orientation index {} but the FRM has only [{}] orientations", direction_index, frm->directions().size());
            direction_index = 0;
        }

        // Take the first frame of the direction
        auto first_frame = frm->directions().at(direction_index).frames().at(0);

        uint16_t left = 0;
        uint16_t top = direction_index * frm->maxFrameHeight();
        uint16_t width = first_frame.width();
        uint16_t height = first_frame.height();

        object_sprite.setTextureRect({ left, top, width, height });

        auto direction = frm->directions().at(direction_index);

        // center on the hex
        object_sprite.setPosition(
            // X
            (float)hex->x() + direction.shiftX() - (width / 2),

            // Y
            (float)hex->y() + direction.shiftY() - height);

        Object entity;
        entity.setSprite(std::move(object_sprite));
        entity.setMapObject(object);

        _objects.push_back(std::move(entity));
    }
}

std::vector<bool> calculateBitset(const sf::Image& img) {
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
        _view.zoom(1.0f - delta * 0.1f);
    }

    // Left mouse click
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {

        const sf::Color BASE_COLOR = { 128, 128, 128 }; // get the current mouse position in the window

        sf::Vector2i pixelPos = sf::Mouse::getPosition(*_appData->window);

        // convert it to world coordinates
        sf::Vector2f worldPos = _appData->window->mapPixelToCoords(pixelPos);

        for (auto& tile : _floorSprites) {
            // mouse is within the tile rect
            if (tile.getGlobalBounds().contains(worldPos)) {

                const auto& image = tile.getTexture()->copyToImage();
                const auto bitsets = calculateBitset(image);

                const auto& x = worldPos.x - tile.getGlobalBounds().left;
                const auto& y = worldPos.y - tile.getGlobalBounds().top;

                spdlog::info("image size {}x{}", image.getSize().x, image.getSize().y);
                spdlog::info("texture size {}x{}", tile.getTexture()->getSize().x, tile.getTexture()->getSize().y);

                spdlog::info("Worldpos x = {}, y = {}", worldPos.x, worldPos.y);
                spdlog::info("Globalbounds x = {}, y = {}", tile.getGlobalBounds().left, tile.getGlobalBounds().top);

                spdlog::info("Pixel x = {}, y = {}", x, y);
                auto pixel = image.getPixel(x, y);

                spdlog::info("Pixel color = {}, {}, {}, {}", pixel.r, pixel.g, pixel.b, pixel.a);
                int w = image.getSize().x;
                if (!bitsets[w * y + x]) {
                    tile.setColor(BASE_COLOR);
                    break;
                }
            }
        }
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
        sf::Vector2f mousePos = sf::Vector2f{ sf::Mouse::getPosition(*_appData->window) - _lastMousePos };
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

void EditorState::update(const float& dt) { }

/**
 * TODO: draw only sprites visible in the current view
 */
void EditorState::render(const float& dt) {

    renderMainMenu();

    _appData->window->setView(_view);

    for (const auto& sprite : _floorSprites) {
        _appData->window->draw(sprite);
    }

    if (_showObjects) {
        for (const auto& obj : _objects) {
            _appData->window->draw(obj.getSprite());
        }
    }

    if (_showRoof) {
        for (const auto& sprite : _roofSprites) {
            _appData->window->draw(sprite);
        }
    }

    //  auto boldFont = io.Fonts->Fonts[0];
    ImGui::PushFont(0);

    // TODO: create a widget
    //    ImGui::Begin("Floor tiles");

    int rowWidth = 0;
    //    for (const auto& floorTile : _floorSprites) {
    //        rowWidth += floorTile.getTextureRect().width;
    //        if (!(rowWidth >= ImGui::GetWindowContentRegionWidth())) {
    //            ImGui::SameLine();
    //        } else {
    //            rowWidth = 0;
    //        }
    //        ImGui::ImageButton(floorTile);
    //    }

    //    ImGui::End();

    //    ImGui::Begin("Roof tiles");
    //    ImGui::End();
    //
    //    ImGui::Begin("Objects");
    //    ImGui::End();
    //
    //    ImGui::Begin("Critters");
    //    ImGui::End();

    ImGui::PopFont();
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
    _appData->mapPath = "test.map";

    int elevations = 1;
    int floorTileIndex = 192; // edg5000.frm
    int roofTileIndex = 1;    // grid000.frm

    _map = std::make_unique<Map>();
    auto map_file = std::make_unique<Map::MapFile>();
    _map->setMapFile(std::move(map_file));

    LstReader lst_reader;
    std::filesystem::path data_path = FileHelper::getInstance().fallout2DataPath();
    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    std::string texture_path = "art/tiles/" + lst->at(floorTileIndex);
    ResourceManager::getInstance().insert(texture_path);

    texture_path = "art/tiles/" + lst->at(roofTileIndex);
    ResourceManager::getInstance().insert(texture_path);

    std::map<int, std::vector<Tile>> tiles;
    for (auto elevation = 0; elevation < elevations; ++elevation) {
        for (auto i = 0U; i < Map::TILES_PER_ELEVATION; ++i) {
            uint16_t roof = roofTileIndex;
            uint16_t floor = floorTileIndex;

            Tile tile(floor, roof);
            tiles[elevation].push_back(tile);
        }
    }
    _map->setTiles(std::move(tiles));

    loadMap();
}

} // namespace geck
