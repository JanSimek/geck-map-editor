#include "Tile.h"

namespace geck {

Tile::Tile(uint16_t floor, uint16_t roof) : _floor(floor), _roof(roof) {}

uint16_t Tile::getFloor() const {
    return _floor; // & 0x0FFF; // 4095 limit, see Dims mapper
}

// see
uint16_t Tile::getRoof() const {
    return _roof; // & 0x0FFF; // 4095 limit, see Dims mapper; roof tile IDs on epax.map
}

}  // namespace geck
