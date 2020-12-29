#pragma once

namespace geck {

class Hex
{
private:
    int _x;
    int _y;
    int _number;
public:
    static constexpr int HEX_WIDTH = 16;
    static constexpr int HEX_HEIGHT = 12;

    Hex(int number, int x, int y);
    Hex(int x, int y);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);
};

}
