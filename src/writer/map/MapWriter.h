#pragma once

#include "../FileWriter.h"
#include "../../format/map/Map.h"
#include "../../reader/pro/ProReader.h"

namespace geck {

class MapWriter : public FileWriter<Map::MapFile> {
public:
    [[nodiscard]] bool write(const Map::MapFile& map);

private:
    ProReader pro_reader;

    void writeScript(const MapScript& script);
    void writeObject(const MapObject& object);
};

} // namespace geck
