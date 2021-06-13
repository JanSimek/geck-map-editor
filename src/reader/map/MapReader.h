#pragma once

#include "../FileReader.h"

namespace geck {

class Map;
class MapObject;
class Pro;

class MapReader : FileReader<Map> {
public:
    enum class ScriptType { system, spatial, timer, item, critter, unknown };

    static constexpr int SCRIPT_SECTIONS = 5;


    std::unique_ptr<Pro> loadPro(unsigned int PID);


private:
    std::unique_ptr<MapObject> readObject(std::istream& stream);
    ScriptType fromPid(uint32_t val);

public:
    using FileReader::read;  // shadowed method
    std::unique_ptr<Map> read(std::istream& stream) override;
};

}  // namespace geck
