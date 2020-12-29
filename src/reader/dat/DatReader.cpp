#include "DatReader.h"

#include <fstream>
#include <stdexcept>

#include "../../util/io.h"
#include "../../format/dat/Dat.h"
#include "../../format/dat/DatEntry.h"

namespace geck {

std::unique_ptr<Dat> DatReader::read(std::istream& stream) {

    using namespace io;

    stream.seekg(-FOOTER_SIZE, stream.end);

    if (stream.bad()) {
        throw std::runtime_error{"Could not seek to the end of a dat2 file"};
    }

    uint32_t tree_size = read_le_u32(stream);
    uint32_t data_size = read_le_u32(stream);

    if (data_size != stream.tellg()) {
        throw std::runtime_error{"Stored file size and real size don't match"};
    }

    // tree_size includes file_cout field size
    uint32_t file_count_offset = data_size - FOOTER_SIZE - tree_size;
    stream.seekg(file_count_offset, stream.beg);

    uint32_t file_count = read_le_u32(stream);

    auto dat = std::make_unique<Dat>();

    // Read DAT entry information
    for (size_t i = 0; i < file_count; ++i) {

        std::unique_ptr<DatEntry> entry = std::make_unique<DatEntry>();

        uint32_t filename_size = read_le_u32(stream);
        std::string filename = read_str(stream, filename_size);
        entry->setFilename(filename);

        bool compressed = static_cast<uint8_t>(read_u8(stream));
        entry->setCompressed(compressed);

        uint32_t unpacked_size = read_le_u32(stream);
        entry->setDecompressedSize(unpacked_size);

        uint32_t packed_size = read_le_u32(stream);
        entry->setPackedSize(packed_size);

        uint32_t data_offset = read_le_u32(stream);
        entry->setOffset(data_offset);

        dat->addEntry(std::move(entry));
    }

    return dat;
}

}
