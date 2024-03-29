#pragma once

#include <inttypes.h>

namespace geck {

class Tile {
private:
    uint16_t _floor;
    uint16_t _roof;

public:
    static constexpr unsigned TILE_WIDTH = 80;
    static constexpr unsigned TILE_HEIGHT = 36;
    static constexpr int ROOF_OFFSET = 96; // "roof height"

    Tile(uint16_t floor, uint16_t roof);

    uint16_t getFloor() const;
    uint16_t getRoof() const;

    void setFloor(uint16_t newFloor);
    void setRoof(uint16_t newRoof);
};

} // namespace geck
