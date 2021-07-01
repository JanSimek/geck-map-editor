#pragma once

#include "../FileWriter.h"
#include "../../format/map/Map.h"

namespace geck {

class MapWriter : public FileWriter<Map::MapFile>
{
public:
    constexpr static MapScript empty_script = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    bool write(const Map::MapFile& map);

private:
    void writeScript(const MapScript& script);
    void writeObject(const MapObject& object);

};

} // namespace geck
