#include "MapLoader.h"
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../../reader/pro/ProReader.h"
#include "../../reader/map/MapReader.h"
#include "../../reader/lst/LstReader.h"

#include "../../format/pro/Pro.h"
#include "../../format/map/Map.h"
#include "../../format/map/Tile.h"
#include "../../format/lst/Lst.h"
#include "../../format/frm/Direction.h"
#include "../../format/frm/Direction.h"

#include "../../util/FileHelper.h"
#include "../../util/ProHelper.h"
#include "../../util/ResourceManager.h"
#include "portable-file-dialogs.h"

namespace geck {

MapLoader::MapLoader(const std::filesystem::path& mapFile, int elevation, std::function<void(std::unique_ptr<Map>)> onLoadCallback)
    : _mapPath(mapFile)
    , _elevation(elevation)
    , _onLoadCallback(onLoadCallback) {
}

void MapLoader::load() {

    // TODO: open from resource manager

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

    spdlog::stopwatch stopwatch_chunk;
    spdlog::stopwatch stopwatch_total;

    setStatus("Loading map " + _mapPath.filename().string());

    LstReader lst_reader{};

    // TODO: move to a new loader that is called only once per application start
    for (const auto& lst_path : { "art/items/items.lst", "art/critters/critters.lst", "art/scenery/scenery.lst", "art/walls/walls.lst", "art/tiles/tiles.lst", "art/misc/misc.lst", "art/intrface/intrface.lst", "art/inven/inven.lst" }) {
        ResourceManager::getInstance().loadResource(lst_path, lst_reader);
    }

    setProgress("Parsing map file");

    MapReader map_reader{ [&](uint32_t PID) {
        ProReader pro_reader;
        return ResourceManager::getInstance().loadResource(ProHelper::basePath(PID), pro_reader);
    } };
    _map = map_reader.openFile(_mapPath);

    if (_elevation == -1) { // TODO: no magic numbers
        uint32_t default_elevation = _map->getMapFile().header.player_default_elevation;
        spdlog::info("Using defaul map elevation {}", default_elevation);
        _elevation = default_elevation;
    }

    spdlog::info("... map file parsed in {:.3} seconds", stopwatch_chunk);
    stopwatch_chunk.reset();

    // Tiles

    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    //    auto tiles = _map->tiles().at(_elevation);
    //    for (auto tileNumber = 0U; tileNumber < geck::Map::TILES_PER_ELEVATION; tileNumber++) {
    //        setProgress("Loading map tile texture " + std::to_string(tileNumber + 1) + " of " + std::to_string(geck::Map::TILES_PER_ELEVATION));
    //        auto tile = tiles.at(tileNumber);
    //        if (tile.getFloor() != Map::EMPTY_TILE) {
    //            ResourceManager::getInstance().insert("art/tiles/" + lst->at(tile.getFloor()));
    //        }
    //        if (tile.getRoof() != Map::EMPTY_TILE) {
    //            ResourceManager::getInstance().insert("art/tiles/" + lst->at(tile.getRoof()));
    //        }
    //    }

    // Tiles
    size_t tile_number = 1;
    size_t tiles_total = lst->list().size();

    for (const auto& tile : lst->list()) {
        setProgress("Loading map tile texture " + std::to_string(tile_number++) + " of " + std::to_string(tiles_total));
        ResourceManager::getInstance().insertTexture("art/tiles/" + tile);
    }

    spdlog::info("... tile textures loaded in {:.3} seconds", stopwatch_chunk);
    stopwatch_chunk.reset();

    // Objects
    size_t objectNumber = 1;
    size_t objectsTotal = _map->objects().at(_elevation).size();

    for (const auto& object : _map->objects().at(_elevation)) {

        setProgress("Loading map object " + std::to_string(objectNumber++) + " of " + std::to_string(objectsTotal));

        if (object->position == -1)
            continue; // object inside an inventory/container

        const std::string frmName = ResourceManager::getInstance().FIDtoFrmName(object->frm_pid);

        ResourceManager::getInstance().insertTexture(frmName);
    }

    ResourceManager::getInstance().insertTexture("art/tiles/blank.frm");

    spdlog::info("... objects loaded in {:.3} seconds", stopwatch_chunk);
    stopwatch_chunk.reset();

    spdlog::info("=======================================");
    spdlog::info("Map loader finished after {:.3} seconds", stopwatch_total);

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
    _onLoadCallback(std::move(_map));
}

} // namespace geck
