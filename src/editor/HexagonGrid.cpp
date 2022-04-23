#include "HexagonGrid.h"

namespace geck {

HexagonGrid::HexagonGrid() {
    // Creating 200x200 hexagonal map
    const unsigned int xMod = Hex::HEX_WIDTH / 2;  // x offset
    const unsigned int yMod = Hex::HEX_HEIGHT / 2; // y offset

    for (unsigned int hy = 0; hy != GRID_HEIGHT; ++hy) // rows
    {
        for (unsigned int hx = 0; hx != GRID_WIDTH; ++hx) // columns
        {
            // Calculate hex's actual position
            const bool oddCol = hx & 1;
            const int oddMod = hy + 1;
            const int x = (48 * (GRID_WIDTH / 2)) + (Hex::HEX_WIDTH * oddMod) - ((Hex::HEX_HEIGHT * 2) * hx) - (xMod * oddCol);
            const int y = (oddMod * Hex::HEX_HEIGHT) + (yMod * hx) + Hex::HEX_HEIGHT - (yMod * oddCol);

            _grid.push_back(std::make_unique<Hex>(x, y));
        }
    }
}

const std::vector<std::unique_ptr<Hex>>& HexagonGrid::grid() const {
    return _grid;
}

} // namespace geck
