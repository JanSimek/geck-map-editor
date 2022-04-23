#include "Hex.h"

namespace geck {

Hex::Hex(int x, int y)
    : _x(x)
    , _y(y) { }

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

} // namespace geck
