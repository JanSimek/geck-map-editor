#include "PalReader.h"

#include <array>

#include "../../format/pal/Pal.h"
#include "../../util/io.h"

namespace geck {

std::unique_ptr<geck::Pal> geck::PalReader::read() {
    constexpr size_t filesize = 0x00008300; // KLAMATH: up to "additional table #1"

    std::array<uint8_t, filesize> buf;
    io::read(_stream, buf.data(), buf.size());

    auto pal = std::make_unique<Pal>();

    // TODO: static colors, e.g. monitors, slime, fire,...
    // https://github.com/falltergeist/falltergeist/blob/39a6cee826c4588cac10919786c939f222684b94/src/Format/Pal/File.cpp

    size_t offset = 0;
    for (Rgb& rgb : pal->palette()) {
        rgb.r = buf[offset++];
        rgb.g = buf[offset++];
        rgb.b = buf[offset++];
    }

    for (uint8_t& conversion : pal->rgbConversionTable()) {
        conversion = buf[offset++];
    }

    return pal;
}

} // namespace geck
