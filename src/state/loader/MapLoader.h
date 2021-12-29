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
    MapLoader(const std::filesystem::path& mapPath);
    ~MapLoader();

    void init() override;
    bool isDone() override;
private:
    void load() override;

    std::atomic<bool> done = false;

    std::filesystem::path _mapPath;
    std::unique_ptr<Map> _map;

public:
    std::unique_ptr<Map> getMap();
};

} // namespace geck
#endif  // GECK_MAPPER_MAPLOADER_H
