#pragma once

#include <map>

#include "../FileParser.h"
#include "../../format/map/MapObject.h"
#include "../../format/map/MapScript.h"
#include "../../format/lst/Lst.h"

namespace geck {

class Map;
class Pro;
class Tile;

class MapReader : public FileParser<Map> {
public:
    enum class ScriptType {
        system,
        spatial,
        timer,
        item,
        critter,
        unknown
    };

    std::unique_ptr<Pro> loadPro(unsigned int PID);

    std::string FIDtoFrmName(unsigned int FID);

    MapReader();

    enum class FRM_TYPE : char {
        ITEM = 0,
        CRITTER,
        SCENERY,
        WALL,
        TILE,
        MISC,
        INTERFACE,
        INVENTORY
    };

private:
    std::unique_ptr<MapObject> readMapObject();
    ScriptType fromPid(uint32_t val);

    // TODO: move to general file/texture/asset manager
    std::unordered_map<FRM_TYPE, std::unique_ptr<Lst>> lst_frm;

public:
    std::unique_ptr<Map> read() override;
};

} // namespace geck
