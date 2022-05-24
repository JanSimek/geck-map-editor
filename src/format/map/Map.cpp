#include "Map.h"

#include <string>

#include "../../util/io.h"
#include "../lst/Lst.h"
#include "../../format/map/MapObject.h"

namespace geck {

const std::unordered_map<int, std::vector<std::shared_ptr<MapObject>>>& Map::objects() const {
    return mapFile->map_objects;
}

Map::MapFile& Map::getMapFile() {
    return *mapFile; //.get();
}

void Map::setMapFile(std::unique_ptr<MapFile> newMapFile) {
    mapFile = std::move(newMapFile);
}

int Map::elevations() const {
    return _tiles.size();
}

} // namespace geck
