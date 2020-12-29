#pragma once

#include <sstream>
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../../editor/Tile.h"
#include "../../editor/MapObject.h"

namespace geck {

//class Tile;
//class MapObject;

class Map
{
private:
    std::map<int, std::vector<Tile> > _tiles;
    std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > > _objects;

public:
    static constexpr unsigned ROWS = 100;
    static constexpr unsigned COLS = 100;
    static constexpr unsigned TILES_PER_ELEVATION = ROWS * COLS;

    const std::map<int, std::vector<Tile> > &tiles() const;
    void setTiles(const std::map<int, std::vector<Tile> > tiles);

    const std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > >& objects() const;
    void setObjects(std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > > objects);

    int elevations() const;
};

}

