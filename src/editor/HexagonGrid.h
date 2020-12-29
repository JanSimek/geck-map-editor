#pragma once

#include <vector>
#include <memory>

#include "Hex.h"

namespace geck {

class HexagonGrid
{
private:
    std::vector<std::unique_ptr<Hex> > _grid;

public:
    static constexpr int GRID_WIDTH = 200;
    static constexpr int GRID_HEIGHT = 200;

    HexagonGrid();

    const std::vector<std::unique_ptr<Hex> >& grid() const;
};

}
