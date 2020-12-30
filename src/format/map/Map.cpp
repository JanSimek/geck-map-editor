#include "Map.h"

#include <stdexcept>
#include <string>

#include "../../util/io.h"
#include "../lst/Lst.h"
#include "../pro/Pro.h"

namespace geck {

void Map::setTiles(const std::map<int, std::vector<Tile> > tiles) {
    _tiles = std::move(tiles);
}

void Map::setObjects(std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > > objects) {
    _objects = std::move(objects);
}

const std::map<int, std::vector<Tile> >& Map::tiles() const {
    return _tiles;
}

const std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > >& Map::objects() const {
    return _objects;
}

int Map::elevations() const {
    return _objects.size();
}

}  // namespace geck
