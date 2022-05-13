#pragma once

#include <cstdint>

namespace geck {

class Hex {
private:
    int _x;
    int _y;
    uint32_t _position;

public:
    static constexpr int HEX_WIDTH = 16;
    static constexpr int HEX_HEIGHT = 12;

    static constexpr int HEX_OUT_OF_MAP = -1;

    Hex(int x, int y, uint32_t position);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);
    uint32_t position() const;
    void setPosition(uint32_t newPosition);
};

} // namespace geck
