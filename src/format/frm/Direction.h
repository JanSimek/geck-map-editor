#pragma once

#include <cstdint>
#include <vector>
#include "Frame.h"

namespace geck {

class Direction {
private:
    int16_t _shiftX = 0;
    int16_t _shiftY = 0;
    uint32_t _dataOffset = 0;
    std::vector<Frame> _frames;

public:
    int16_t shiftX() const;
    void setShiftX(const int16_t& shiftX);

    int16_t shiftY() const;
    void setShiftY(const int16_t& shiftY);

    uint32_t dataOffset() const;
    void setDataOffset(const uint32_t& dataOffset);

    const std::vector<Frame>& frames() const;
    void setFrames(std::vector<Frame> frames);

    uint16_t width() const;
    uint16_t height() const;
};

} // namespace geck
