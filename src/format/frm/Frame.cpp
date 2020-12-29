#include "Frame.h"

namespace geck {

uint16_t Frame::width() const
{
    return _width;
}

void Frame::setWidth(const uint16_t &width)
{
    _width = width;
}

uint16_t Frame::height() const
{
    return _height;
}

void Frame::setHeight(const uint16_t &height)
{
    _height = height;
}

int16_t Frame::offsetX() const
{
    return _offsetX;
}

void Frame::setOffsetX(const int16_t &offsetX)
{
    _offsetX = offsetX;
}

int16_t Frame::offsetY() const
{
    return _offsetY;
}

void Frame::setOffsetY(const int16_t &offsetY)
{
    _offsetY = offsetY;
}

uint8_t Frame::index(uint16_t x, uint16_t y) const
{
    if (x >= _width || y >= _height) return 0;

    // FIXME: stupid cast to silence compiler error
    return _indexes.at(static_cast<size_t>(_width*y + x));
}

uint8_t* Frame::data()
{
    return _indexes.data();
}

Frame::Frame(uint16_t width, uint16_t height) : _indexes(width * height, 0)
{
    _width = width;
    _height = height;

    // TODO: _indexes.resize()
}

}
