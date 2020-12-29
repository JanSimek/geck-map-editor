#include "Tile.h"

namespace geck {

Tile::Tile(int floor, int roof)
    : _floor(floor), _roof(roof)
{

}

int Tile::getFloor() const
{
    return _floor;
}

int Tile::getRoof() const
{
    return _roof;
}

}
