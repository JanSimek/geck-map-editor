//
// Created by jsimek on 27.07.21.
//

#ifndef GECK_MAPPER_MAPLOADER_H
#define GECK_MAPPER_MAPLOADER_H

#include <filesystem>
#include <thread>
#include "Loader.h"

namespace geck {

class Map;

class MapLoader : public Loader {
public:
    MapLoader(const std::filesystem::path& mapPath, int elevation);
    ~MapLoader();

    std::unique_ptr<Map> getMap();

    void init() override;
    bool isDone() override;
private:
    void load() override;

    std::atomic<bool> done = false;

    std::filesystem::path _mapPath;
    std::unique_ptr<Map> _map;
    int _elevation;

};

} // namespace geck
#endif  // GECK_MAPPER_MAPLOADER_H
