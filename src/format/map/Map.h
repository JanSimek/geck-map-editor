#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../format/map/MapObject.h"
#include "../../editor/Tile.h"
#include "../../editor/Object.h"
#include "MapScript.h"

namespace geck {

// class Tile;
// class MapObject;

class Map {
public:
    static constexpr unsigned ROWS = 100;
    static constexpr unsigned COLS = 100;
    static constexpr unsigned TILES_PER_ELEVATION = ROWS * COLS;

    static constexpr int SCRIPT_SECTIONS = 5;

    // Header
    struct MapHeader {
        uint32_t version;
        std::string filename;

        uint32_t player_default_position;
        uint32_t player_default_elevation; // map elevation where the map is loaded
        uint32_t player_default_orientation;

        uint32_t num_local_vars;
        int32_t script_id;
        uint32_t flags;
        uint32_t darkness;
        uint32_t num_global_vars;
        uint32_t map_id;
        uint32_t timestamp;
    };

    struct MapFile {
        MapHeader header;

        // Global and local variables
        std::vector<uint32_t> map_local_vars;
        std::vector<uint32_t> map_global_vars;

        // Tiles
        std::map<int, std::vector<Tile> > tiles;

        // Scripts
        std::vector<MapScript> map_scripts[SCRIPT_SECTIONS];
        std::array<int, SCRIPT_SECTIONS> scripts_in_section;

        // Objects sorted by elevation
        std::unordered_map<int, std::vector<std::shared_ptr<MapObject> >> map_objects;
    };

    const std::map<int, std::vector<Tile> >& tiles() const;
    void setTiles(const std::map<int, std::vector<Tile> >& tiles);

    const std::unordered_map<int, std::vector<std::shared_ptr<MapObject> > >& objects() const;

    int elevations() const;

    const Map::MapFile& getMapFile() const;
    void setMapFile(std::unique_ptr<MapFile> newMapFile);

private:

    std::unique_ptr<MapFile> mapFile;

    std::map<int, std::vector<Tile> > _tiles;
//    std::unordered_map<int, std::vector<std::unique_ptr<Object> > > _objects;
};

}  // namespace geck
