#include "FrmReader.h"

#include "../../format/frm/Direction.h"
#include "../../format/frm/Frame.h"
#include "../../format/frm/Frm.h"

namespace geck {

std::unique_ptr<geck::Frm> geck::FrmReader::read(/*std::istream& stream*/) {
    auto frm = std::make_unique<Frm>();
    frm->setVersion(read_be_u32());
    frm->setFps(read_be_u16());
    frm->setActionFrame(read_be_u16());
    frm->setFramesPerDirection(read_be_u16());

    uint16_t shiftX[Frm::ORIENTATIONS];
    uint16_t shiftY[Frm::ORIENTATIONS];
    uint32_t dataOffset[Frm::ORIENTATIONS];
    for (unsigned int i = 0; i != Frm::ORIENTATIONS; ++i)
        shiftX[i] = read_be_u16();
    for (unsigned int i = 0; i != Frm::ORIENTATIONS; ++i)
        shiftY[i] = read_be_u16();
    for (unsigned int i = 0; i != Frm::ORIENTATIONS; ++i) {
        dataOffset[i] = read_be_u32();
        if (i > 0 && dataOffset[i - 1] == dataOffset[i]) {
            continue;
        }

        frm->orientations().emplace_back();
        auto& direction = frm->orientations().back();
        direction.setDataOffset(dataOffset[i]);
        direction.setShiftX(shiftX[i]);
        direction.setShiftY(shiftY[i]);
    }

    uint32_t size_of_frame_data = read_be_u32();

    auto data_start = stream.tellg();
    stream.seekg(size_of_frame_data, std::ios_base::cur);
    auto data_end = stream.tellg();

    if (data_end - data_start < size_of_frame_data) {
        throw std::runtime_error{
            "invalid frm data: the size of the frame data (indicated in header) is smaller than the provided data"};
    }

    // for each direction
    for (auto& direction : frm->orientations()) {
        // jump to frames data at frames area
        std::streamoff frame_data_offset = data_start + static_cast<std::streamoff>(direction.dataOffset());

        stream.seekg(frame_data_offset, std::ios_base::beg);

        // read all frames
        for (unsigned i = 0; i != frm->framesPerDirection(); ++i) {
            uint16_t width = read_be_u16();
            uint16_t height = read_be_u16();

            Frame frame(width, height);

            //            auto& frame = direction.frames().back();

            // Number of pixels for this frame
            // We don't need this, because we already have width*height
            read_be_u32();

            frame.setOffsetX(read_be_u16());
            frame.setOffsetY(read_be_u16());

            // Pixels data
            stream.read((char*)frame.data(), frame.width() * frame.height());

            direction.frames().emplace_back(frame);
        }
    }

    return frm;
}

}  // namespace geck
