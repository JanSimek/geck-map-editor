#include "FrmReader.h"

#include "../../format/frm/Direction.h"
#include "../../format/frm/Frame.h"
#include "../../format/frm/Frm.h"

namespace geck {

std::unique_ptr<geck::Frm> geck::FrmReader::read() {
    auto frm = std::make_unique<Frm>(_path);
    frm->setVersion(read_be_u32());
    frm->setFps(read_be_u16());
    frm->setActionFrame(read_be_u16());
    frm->setFramesPerDirection(read_be_u16());

    uint16_t shiftX[Frm::DIRECTIONS];
    uint16_t shiftY[Frm::DIRECTIONS];
    uint32_t dataOffset[Frm::DIRECTIONS];

    std::vector<Direction> directions;
    for (unsigned int i = 0; i != Frm::DIRECTIONS; ++i)
        shiftX[i] = read_be_u16();
    for (unsigned int i = 0; i != Frm::DIRECTIONS; ++i)
        shiftY[i] = read_be_u16();
    for (unsigned int i = 0; i != Frm::DIRECTIONS; ++i) {
        dataOffset[i] = read_be_u32();
        if (i > 0 && dataOffset[i - 1] == dataOffset[i]) {
            continue;
        }

        Direction direction{};
        direction.setDataOffset(dataOffset[i]);
        direction.setShiftX(shiftX[i]);
        direction.setShiftY(shiftY[i]);

        directions.emplace_back(direction);
    }

    uint32_t size_of_frame_data = read_be_u32();

    auto data_start = _stream.position();
    _stream.setPosition(data_start + size_of_frame_data);
    auto data_end = _stream.position();

    if (data_end - data_start < size_of_frame_data) {
        throw std::runtime_error{
            "invalid frm data: the size of the frame data (indicated in header) is smaller than the provided data"
        };
    }

    // for each direction
    for (auto& direction : directions) {
        // jump to frames data at frames area
        std::streamoff frame_data_offset = data_start + static_cast<std::streamoff>(direction.dataOffset());

        _stream.setPosition(frame_data_offset);

        // read all frames
        std::vector<Frame> frames;
        for (unsigned i = 0; i != frm->framesPerDirection(); ++i) {
            uint16_t width = read_be_u16();
            uint16_t height = read_be_u16();

            Frame frame(width, height);

            // auto& frame = direction.frames().back();

            // Number of pixels for this frame
            // We don't need this, because we already have width*height
            read_be_u32(); // todo check = W*H

            frame.setOffsetX(read_be_u16());
            frame.setOffsetY(read_be_u16());

            // Pixels data
            _stream.read(frame.data(), frame.width() * frame.height());

            frames.emplace_back(frame);
        }
        direction.setFrames(frames);
    }

    frm->setDirections(directions);

    return frm;
}

} // namespace geck
