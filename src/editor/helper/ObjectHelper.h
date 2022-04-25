#pragma once

#include "../../format/pro/Pro.h"

namespace geck {

class ObjectHelper {

public:
    static const std::string objectTypeFromId(uint32_t id) {
        Pro::OBJECT_TYPE object_type = static_cast<Pro::OBJECT_TYPE>(id);
        switch (object_type) {
            case Pro::OBJECT_TYPE::ITEM:
                return "ITEM";
            case Pro::OBJECT_TYPE::CRITTER:
                return "CRITTER";
            case Pro::OBJECT_TYPE::SCENERY:
                return "SCENERY";
            case Pro::OBJECT_TYPE::WALL:
                return "WALL";
            case Pro::OBJECT_TYPE::TILE:
                return "TILE";
            case Pro::OBJECT_TYPE::MISC:
                return "MISC";
            default:
                return "unknown";
        }
    }
};

} // namespace geck
