#include "MapLoader.h"
#include <thread>
#include <spdlog/spdlog.h>

#include "../../reader/map/MapReader.h"
#include "../../format/map/Map.h"
#include "../../format/lst/Lst.h"
#include "../../format/frm/Direction.h"
#include "../../reader/lst/LstReader.h"
#include "../../util/FileHelper.h"
#include "../../util/TextureManager.h"
#include "portable-file-dialogs.h"

namespace geck {

MapLoader::MapLoader(const std::filesystem::path& mapFile) : _mapPath(mapFile) {
}

void MapLoader::load() {

    TextureManager::getInstance().setDataPath(FileHelper::getInstance().resourcesPath()); // FIXME: move
    const auto data_path = FileHelper::getInstance().fallout2DataPath();

//    std::filesystem::path data_path = FileHelper::getInstance().fallout2DataPath();

    if (_mapPath.empty()) {
        _mapPath = pfd::open_file("Choose Fallout 2 map to load", data_path,
                                 { "Fallout 2 map (.map)", "*.map" },
                                 pfd::opt::none).result().front();

        if (_mapPath.empty()) {
            spdlog::error("You must choose a Fallout 2 map file to load");
            return;
        }

//        _appData->mapName = std::filesystem::path(mapFile).filename();
    }

    MapReader map_reader;
    _map = map_reader.openFile(_mapPath);

    LstReader lst_reader;
    auto lst = lst_reader.openFile(data_path / "art/tiles/tiles.lst");

    for (int elevation = 0; elevation < _map->elevations(); elevation++) {

        setStatus("Loading map " + _mapPath.filename().string() + " elevation #" + std::to_string(elevation + 1));

        // Tiles
        for (auto tileNumber = 0U; tileNumber < geck::Map::TILES_PER_ELEVATION; ++tileNumber) {

            setProgress("Loading map tile " + std::to_string(tileNumber + 1) + " of " + std::to_string(geck::Map::TILES_PER_ELEVATION));

            auto tile = _map->tiles().at(elevation).at(tileNumber);

            // floor
            std::string floor_texture_path = "art/tiles/" + lst->at(tile.getFloor());
            TextureManager::getInstance().insert(floor_texture_path);

            // roof
            std::string roof_texture_path = "art/tiles/" + lst->at(tile.getRoof());
            TextureManager::getInstance().insert(roof_texture_path);
        }

        // Objects
        size_t objectNumber = 1;
        size_t objectsTotal = _map->objects().at(elevation).size();

        for (const auto& object : _map->objects().at(elevation)) {

            setProgress("Loading map object " + std::to_string(objectNumber++) + " of " + std::to_string(objectsTotal));

            if (object->position == -1)
                continue;  // object inside an inventory/container

            const std::string frmName = map_reader.FIDtoFrmName(object->frm_pid);

            if (frmName.empty()) {
                spdlog::error("Empty FRM file path on hex number " + std::to_string(object->position));
                continue;  // this should probably never happen
            }

            TextureManager::getInstance().insert(frmName, object->orientation);
        }

    }
    done = true;
}

MapLoader::~MapLoader() {
    if (_thread.joinable())
        _thread.join();
}

void MapLoader::init() {
    _thread = std::thread{&MapLoader::load, this};
}

bool MapLoader::isDone() {
    return done;
}

std::unique_ptr<Map> MapLoader::getMap()  {
    return std::move(_map); // release ownership
}

} // namespace geck