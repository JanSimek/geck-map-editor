#include "Map.h"

#include <string>

#include "../../util/io.h"
#include "../lst/Lst.h"
#include "../../format/map/MapObject.h"
#include "../../editor/Tile.h"

namespace geck {

void Map::setTiles(const std::map<int, std::vector<Tile>>& tiles) {
    _tiles = std::move(tiles);
}

const std::map<int, std::vector<Tile>>& Map::tiles() const {
    return _tiles;
}

const std::unordered_map<int, std::vector<std::shared_ptr<MapObject>>>& Map::objects() const {
    return mapFile->map_objects;
}

const Map::MapFile& Map::getMapFile() const {
    return *mapFile; //.get();
}

void Map::setMapFile(std::unique_ptr<MapFile> newMapFile) {
    mapFile = std::move(newMapFile);
}

int Map::elevations() const {
    return _tiles.size();
}

} // namespace geck
