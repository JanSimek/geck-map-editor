#include <algorithm>

#include "Direction.h"
#include "Frame.h"

namespace geck {

int16_t Direction::shiftX() const
{
    return _shiftX;
}

void Direction::setShiftX(const int16_t &shiftX)
{
    _shiftX = shiftX;
}

int16_t Direction::shiftY() const
{
    return _shiftY;
}

void Direction::setShiftY(const int16_t &shiftY)
{
    _shiftY = shiftY;
}

uint32_t Direction::dataOffset() const
{
    return _dataOffset;
}

void Direction::setDataOffset(const uint32_t &dataOffset)
{
    _dataOffset = dataOffset;
}

std::vector<Frame>& Direction::frames()
{
    return _frames;
}

void Direction::setFrames(const std::vector<Frame> &frames)
{
    _frames = frames;
}

uint16_t Direction::width() const
{
    auto widest = std::max_element(_frames.begin(), _frames.end(), [](const Frame& a, const Frame& b) {
        return a.width() < b.width();
    });

    return static_cast<uint16_t>((widest->width() + 2) * _frames.size());
}

uint16_t Direction::height() const
{
    auto tallest = std::max_element(_frames.begin(), _frames.end(), [](const Frame& a, const Frame& b)
    {
        return a.height() < b.height();
    });
    return tallest->height() + 2;
}

}
