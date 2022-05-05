#include "Hex.h"

namespace geck {

Hex::Hex(int x, int y, uint32_t position)
    : _x(x)
    , _y(y)
    , _position(position) { }

int Hex::y() const {
    return _y;
}

void Hex::setY(int y) {
    _y = y;
}

int Hex::x() const {
    return _x;
}

void Hex::setX(int x) {
    _x = x;
}

uint32_t Hex::position() const {
    return _position;
}

void Hex::setPosition(uint32_t newPosition) {
    _position = newPosition;
}

} // namespace geck
