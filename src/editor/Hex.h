#pragma once

namespace geck {

class Hex {
private:
    int _x;
    int _y;

public:
    static constexpr int HEX_WIDTH = 16;
    static constexpr int HEX_HEIGHT = 12;

    Hex(int x, int y);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);
};

} // namespace geck
