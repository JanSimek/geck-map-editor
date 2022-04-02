#include "Frm.h"
#include "Direction.h"
#include "Frame.h"
#include "../pal/Pal.h"

#include <numeric>
#include <algorithm>

#include <spdlog/spdlog.h>

namespace geck {

const std::vector<Direction>& Frm::directions() const {
    return _directions;
}

void Frm::setDirections(std::vector<Direction> directions) {
    _directions = std::move(directions);

    updateFrameDimensions();
}

uint32_t Frm::version() const {
    return _version;
}

void Frm::setVersion(const uint32_t& version) {
    _version = version;
}

uint16_t Frm::fps() const {
    return _fps;
}

void Frm::setFps(const uint16_t& fps) {
    _fps = fps;
}

uint16_t Frm::actionFrame() const {
    return _actionFrame;
}

void Frm::setActionFrame(const uint16_t& actionFrame) {
    _actionFrame = actionFrame;
}

uint16_t Frm::framesPerDirection() const {
    return _framesPerDirection;
}

void Frm::setFramesPerDirection(const uint16_t& framesPerDirection) {
    _framesPerDirection = framesPerDirection;
}

/**
 * @brief Calculates total FRM width by multiplying the widest direction by the number of directions
 * @return width of the widest direction
 */
uint16_t Frm::width() const
{
    auto widest = std::max_element(_directions.begin(), _directions.end(), [](const Direction& a, const Direction& b)
    {
        return a.width() < b.width();
    });

    return widest->width() * _directions.size();
}

/**
 * @brief Calculates total FRM height by multiplying the tallest direction by the number of directions
 * @return height of the tallest direction
 */
uint16_t Frm::height() const
{
    auto tallest = std::max_element(_directions.begin(), _directions.end(), [](const Direction& a, const Direction& b)
    {
        return a.height() < b.height();
    });

    return tallest->height() * _directions.size();
}

void Frm::updateFrameDimensions()
{
    _maxFrameWidth = _directions.front().frames().front().width();
    _maxFrameHeight = _directions.front().frames().front().height();

    for (Direction& direction : _directions) {

        spdlog::info("Direction has {} frames", direction.frames().size());
        for (const Frame& frame : direction.frames()) {
            if (frame.width() > _maxFrameWidth)
                _maxFrameWidth = frame.width();
            if (frame.height() > _maxFrameHeight)
                _maxFrameHeight = frame.height();
        }
    }
}

uint16_t Frm::maxFrameWidth() const
{
    return _maxFrameWidth;
}

uint16_t Frm::maxFrameHeight() const
{
    return _maxFrameHeight;
}

const sf::Image& Frm::image(Pal *pal)
{
    if (_image.getSize() != sf::Vector2u{0, 0}) {
        return _image;
    }

    constexpr int RGBA = 4;

    auto colors = pal->palette();

    // find maximum width and height
    unsigned maxWidth = maxFrameWidth();
    unsigned maxHeight = maxFrameHeight();

    spdlog::info("FRM total width = {}, total height = {}", width(), height());
    spdlog::info("FRM max frame width = {}, max frame height = {}", maxWidth, maxHeight);

    _image = sf::Image{};
    _image.create(width(), height(), { 0, 0, 0, 0});

    int yOffset = 0;
    for (Direction& direction : _directions) {
        int xOffset = 0;

        for (const Frame& frame : direction.frames()) {

            for (int x = 0; x < frame.width(); x++) {
                for (int y = 0; y < frame.height(); y++) {

                    uint8_t paletteIndex = frame.index(x, y);
                    geck::Rgb color = colors[paletteIndex];

                    constexpr uint8_t white = 255;
                    constexpr uint8_t opaque_alpha = 255;

                    uint8_t r, g, b, a;
                    if (color.r == white && color.g == white && color.b == white) {
                        // transparent
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    } else {
                        constexpr int brightness = 4;  // brightness modifier
                        r = color.r * brightness;
                        g = color.g * brightness;
                        b = color.b * brightness;
                        a = opaque_alpha;
                    }

                    _image.setPixel(
                                maxWidth * xOffset + x,
                                maxHeight * yOffset + y,
                                { r, g, b, a });
                }
            }
            xOffset++;
        }
        yOffset++;
    }

    return _image;
}


}  // namespace geck
