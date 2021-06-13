#include "Tile.h"

namespace geck {

Tile::Tile(uint16_t floor, uint16_t roof) : _floor(floor), _roof(roof) {}

uint16_t Tile::getFloor() const {
    return _floor;
}

uint16_t Tile::getRoof() const {
    return _roof;
}

}  // namespace geck
