#include "HexagonGrid.h"
#include "Hex.h"

namespace geck {

HexagonGrid::HexagonGrid() {
    // Creating 200x200 hexagonal map
    const unsigned int xMod = Hex::HEX_WIDTH / 2;  // x offset
    const unsigned int yMod = Hex::HEX_HEIGHT / 2; // y offset

    uint32_t position = 0;
    for (unsigned int hy = 0; hy != GRID_HEIGHT; ++hy) // rows
    {
        for (unsigned int hx = 0; hx != GRID_WIDTH; ++hx) // columns
        {
            // Calculate hex's actual position
            const bool oddCol = hx & 1;
            const int oddMod = hy + 1;
            const int x = (48 * (GRID_WIDTH / 2)) + (Hex::HEX_WIDTH * oddMod) - ((Hex::HEX_HEIGHT * 2) * hx) - (xMod * oddCol);
            const int y = (oddMod * Hex::HEX_HEIGHT) + (yMod * hx) + Hex::HEX_HEIGHT - (yMod * oddCol);

            _grid.emplace_back(x, y, position);
            ++position;
        }
    }
}

const std::vector<Hex>& HexagonGrid::grid() const {
    return _grid;
}

uint32_t HexagonGrid::positionAt(uint32_t x, uint32_t y) {
    for (size_t i = 0; i < _grid.size(); i++) {
        const auto& hex = _grid.at(i);
        if (x >= hex.x() - Hex::HEX_WIDTH && x < hex.x() + Hex::HEX_WIDTH && y >= hex.y() - 8 && y < hex.y() + 4) {
            return i;
        }
    }
    return Hex::HEX_OUT_OF_MAP;
}

} // namespace geck
