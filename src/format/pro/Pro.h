#pragma once

#include <istream>

namespace geck {

class Pro {
private:
    unsigned int _objectSubtypeId;
    uint32_t _messageId;

public:
    enum class OBJECT_TYPE : uint32_t {
        ITEM = 0,
        CRITTER,
        SCENERY,
        WALL,
        TILE,
        MISC
    };

    enum class ITEM_TYPE : uint32_t {
        ARMOR = 0,
        CONTAINER,
        DRUG,
        WEAPON,
        AMMO,
        MISC,
        KEY
    };

    enum class SCENERY_TYPE : uint32_t {
        DOOR = 0,
        STAIRS,
        ELEVATOR,
        LADDER_BOTTOM,
        LADDER_TOP,
        GENERIC
    };

    unsigned int objectSubtypeId();
    void setObjectSubtypeId(unsigned int objectSubtypeId);

    uint32_t messageId() const;
    void setMessageId(uint32_t newMessageId);
};

} // namespace geck
