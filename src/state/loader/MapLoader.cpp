#include "MapLoader.h"
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../../reader/map/MapReader.h"
#include "../../format/map/Map.h"
#include "../../format/lst/Lst.h"
#include "../../editor/Tile.h"
#include "../../format/frm/Direction.h"
#include "../../reader/lst/LstReader.h"
#include "../../util/FileHelper.h"
#include "../../util/ResourceManager.h"
#include "portable-file-dialogs.h"

namespace geck {

MapLoader::MapLoader(const std::filesystem::path& mapFile, int elevation, std::function<void(std::unique_ptr<Map>)> onLoad)
    : _mapPath(mapFile)
    , _elevation(elevation)
    , _onLoad(onLoad) {
}

void MapLoader::load() {

    // TODO: open from resource manager
    spdlog::stopwatch sw;

    ResourceManager::getInstance().setDataPath(FileHelper::getInstance().fallout2DataPath()); // FIXME: move
    const auto data_path = FileHelper::getInstance().fallout2DataPath();

    if (_mapPath.empty()) {
        _mapPath = pfd::open_file("Choose Fallout 2 map to load", data_path.string(),
            { "Fallout 2 map (.map)", "*.map" },
            pfd::opt::none)
                       .result()
                       .front();

        if (_mapPath.empty()) {
            // TODO: go back to the initial screen
            spdlog::error("You must choose a Fallout 2 map file to load");
            return;
        }
    }

    setStatus("Loading map " + _mapPath.filename().string());

    setProgress("Parsing map file");

    MapReader map_reader;
    _map = map_reader.openFile(_mapPath);

    LstReader lst_reader;
    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    if (_elevation == -1) { // TODO: no magic numbers
        uint32_t default_elevation = _map->getMapFile().header.player_default_elevation;
        spdlog::info("Using defaul map elevation {}", default_elevation);
        _elevation = default_elevation;
    }

    // Tiles
    auto tiles = _map->tiles().at(_elevation);
    for (auto tileNumber = 0U; tileNumber < geck::Map::TILES_PER_ELEVATION; tileNumber++) {

        setProgress("Loading map tile texture " + std::to_string(tileNumber + 1) + " of " + std::to_string(geck::Map::TILES_PER_ELEVATION));

        auto tile = tiles.at(tileNumber);

        // floor
        if (tile.getFloor() != Map::EMPTY_TILE) {
            ResourceManager::getInstance().insert("art/tiles/" + lst->at(tile.getFloor()));
        }

        // roof
        if (tile.getRoof() != Map::EMPTY_TILE) {
            ResourceManager::getInstance().insert("art/tiles/" + lst->at(tile.getRoof()));
        }
    }

    // Objects
    size_t objectNumber = 1;
    size_t objectsTotal = _map->objects().at(_elevation).size();

    for (const auto& object : _map->objects().at(_elevation)) {

        setProgress("Loading map object " + std::to_string(objectNumber++) + " of " + std::to_string(objectsTotal));

        if (object->position == -1)
            continue; // object inside an inventory/container

        const std::string frmName = map_reader.FIDtoFrmName(object->frm_pid);

        ResourceManager::getInstance().insert(frmName);
    }

    spdlog::info("Map loader finished after {:.3} seconds", sw);

    done = true;
}

MapLoader::~MapLoader() {
    if (_thread.joinable())
        _thread.join();
}

void MapLoader::init() {
    _thread = std::thread{ &MapLoader::load, this };
}

bool MapLoader::isDone() {
    return done;
}

void MapLoader::onDone() {
    _onLoad(std::move(_map));
}

} // namespace geck
