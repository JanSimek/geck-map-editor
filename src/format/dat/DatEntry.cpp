#include "DatEntry.h"

namespace geck {

std::string DatEntry::getFilename() const {
    return filename;
}

void DatEntry::setFilename(const std::string& value) {
    filename = value;
}

bool DatEntry::getCompressed() const {
    return compressed;
}

void DatEntry::setCompressed(bool value) {
    compressed = value;
}

uint32_t DatEntry::getDecompressedSize() const {
    return decompressedSize;
}

void DatEntry::setDecompressedSize(const uint32_t& value) {
    decompressedSize = value;
}

uint32_t DatEntry::getPackedSize() const {
    return packedSize;
}

void DatEntry::setPackedSize(const uint32_t& value) {
    packedSize = value;
}

uint32_t DatEntry::getOffset() const {
    return offset;
}

void DatEntry::setOffset(const uint32_t& value) {
    offset = value;
}

} // namespace geck
