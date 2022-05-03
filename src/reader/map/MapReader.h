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
    const std::string FIDtoFrmName(unsigned int FID) const;

    MapReader(const std::filesystem::path& dataPath);

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
    std::filesystem::path _dataPath;
    std::unique_ptr<MapObject> readMapObject();
    MapScript::ScriptType fromPid(uint32_t val);

    // TODO: move to general file/texture/asset manager
    std::unordered_map<FRM_TYPE, std::unique_ptr<Lst>> lst_frm;

public:
    std::unique_ptr<Map> read() override;
};

} // namespace geck
