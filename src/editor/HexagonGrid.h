#pragma once

#include <memory>
#include <vector>

namespace geck {

class Hex;

class HexagonGrid {
private:
    std::vector<Hex> _grid;

public:
    static constexpr int GRID_WIDTH = 200;
    static constexpr int GRID_HEIGHT = 200;

    HexagonGrid();

    const std::vector<Hex>& grid() const;

    uint32_t positionAt(uint32_t x, uint32_t y);
};

} // namespace geck
