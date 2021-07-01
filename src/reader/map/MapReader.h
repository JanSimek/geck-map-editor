#pragma once

#include <map>

#include "../FileParser.h"
#include "../../format/map/MapObject.h"
#include "../../format/map/MapScript.h"

namespace geck {

class Map;
class Pro;
class Tile;

class MapReader : public FileParser<Map> {
public:
    enum class ScriptType { system, spatial, timer, item, critter, unknown };

    std::unique_ptr<Pro> loadPro(unsigned int PID);

    std::string FIDtoFrmName(unsigned int FID);
private:
    std::unique_ptr<MapObject> readMapObject();
    ScriptType fromPid(uint32_t val);

public:
    std::unique_ptr<Map> read() override;
};

}  // namespace geck
