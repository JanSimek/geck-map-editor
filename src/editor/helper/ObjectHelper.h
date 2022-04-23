#pragma once

#include "../Object.h"

namespace geck {

class ObjectHelper {

public:
    static const std::string objectTypeFromId(uint32_t id) {
        Object::OBJECT_TYPE object_type = static_cast<Object::OBJECT_TYPE>(id);
        switch (object_type) {
            case Object::OBJECT_TYPE::ITEM:
                return "ITEM";
            case Object::OBJECT_TYPE::CRITTER:
                return "CRITTER";
            case Object::OBJECT_TYPE::SCENERY:
                return "SCENERY";
            case Object::OBJECT_TYPE::WALL:
                return "WALL";
            case Object::OBJECT_TYPE::TILE:
                return "TILE";
            case Object::OBJECT_TYPE::MISC:
                return "MISC";
            default:
                return "unknown";
        }
    }
};

} // namespace geck
