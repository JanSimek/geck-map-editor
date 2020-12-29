#include "Frm.h"
#include "Direction.h"
#include "Frame.h"
#include "../pal/Pal.h"
#include "../../util/io.h"

#include <algorithm>

namespace geck {

std::vector<Direction>& Frm::directions()
{
    return _directions;
}

void Frm::setDirections(const std::vector<Direction> &directions)
{
    _directions = directions;
}

std::string Frm::path() const
{
    return _path;
}

uint32_t Frm::version() const
{
    return _version;
}

void Frm::setVersion(const uint32_t &version)
{
    _version = version;
}

uint16_t Frm::fps() const
{
    return _fps;
}

void Frm::setFps(const uint16_t &fps)
{
    _fps = fps;
}

uint16_t Frm::actionFrame() const
{
    return _actionFrame;
}

void Frm::setActionFrame(const uint16_t &actionFrame)
{
    _actionFrame = actionFrame;
}

uint16_t Frm::framesPerDirection() const
{
    return _framesPerDirection;
}

void Frm::setFramesPerDirection(const uint16_t &framesPerDirection)
{
    _framesPerDirection = framesPerDirection;
}

}
