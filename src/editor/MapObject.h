#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace geck {

class MapObject {
private:
    size_t _inventory_size;
    size_t _amount;

    uint32_t _FID;

    uint32_t _PID;
    unsigned int _objectSubtypeId;
    int32_t _hexPosition;  // -1 = not on the map

    uint32_t _frmTypeId;
    uint32_t _frmId;

    // TODO: not here
    std::string _frm;

public:
    enum class OBJECT_TYPE : uint32_t { ITEM = 0, CRITTER, SCENERY, WALL, TILE, MISC };

    enum class ITEM_TYPE : uint32_t { ARMOR = 0, CONTAINER, DRUG, WEAPON, AMMO, MISC, KEY };

    enum class SCENERY_TYPE : uint32_t { DOOR = 0, STAIRS, ELEVATOR, LADDER_BOTTOM, LADDER_TOP, GENERIC };

    size_t inventory_size() const;
    void setInventory_size(const size_t& inventory_size);

    size_t amount() const;
    void setAmount(const size_t& amount);

    uint32_t PID() const;
    void setPID(const uint32_t& PID);

    unsigned int objectSubtypeId() const;
    void setObjectSubtypeId(unsigned int objectSubtypeId);

    int32_t hexPosition() const;
    void setHexPosition(const int32_t& hexPosition);

    std::string frm() const;
    void setFrm(const std::string& frm);

    uint32_t FID() const;
    void setFID(const uint32_t& FID);
};

}  // namespace geck
