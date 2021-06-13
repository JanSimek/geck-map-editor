#pragma once

#include <fstream>
#include <string>
#include <vector>

//#include "Direction.h"

namespace geck {

class Direction;
class Pal;

class Frm {
private:
    uint32_t _version;
    uint16_t _fps;
    uint16_t _actionFrame;
    uint16_t _framesPerDirection;

    std::vector<Direction> _directions;

    std::string _path;

public:
    // TODO : enum
    //    north_east,
    //    east,
    //    south_east,
    //    south_west,
    //    west,
    //    north_west,
    constexpr static int ORIENTATIONS = 6;

    std::vector<Direction>& orientations();
    void setOrientations(const std::vector<Direction>& orientations);

    std::string path() const;

    uint32_t version() const;
    void setVersion(const uint32_t& version);

    uint16_t fps() const;
    void setFps(const uint16_t& fps);

    uint16_t actionFrame() const;
    void setActionFrame(const uint16_t& actionFrame);

    uint16_t framesPerDirection() const;
    void setFramesPerDirection(const uint16_t& framesPerDirection);
};

}  // namespace geck
