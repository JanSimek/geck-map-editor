#pragma once

#include "../FileWriter.h"
#include "../../format/map/Map.h"
#include "../../reader/pro/ProReader.h"

namespace geck {

class MapWriter : public FileWriter<Map::MapFile> {
public:
    MapWriter(const std::filesystem::path& dataPath)
        : _dataPath(dataPath) { }

    [[nodiscard]] bool write(const Map::MapFile& map);

private:
    ProReader _proReader;
    std::filesystem::path _dataPath;

    void writeScript(const MapScript& script);
    void writeObject(const MapObject& object);
};

} // namespace geck
