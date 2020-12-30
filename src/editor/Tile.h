#pragma once

namespace geck {

class Tile {
private:
    int _floor;
    int _roof;

public:
    static constexpr unsigned TILE_WIDTH = 80;
    static constexpr unsigned TILE_HEIGHT = 36;

    Tile(int floor, int roof);

    int getFloor() const;
    int getRoof() const;
};

}  // namespace geck
