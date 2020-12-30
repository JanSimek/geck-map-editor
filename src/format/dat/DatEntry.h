#pragma once

#include <string>

namespace geck {

class DatEntry {
private:
    std::string filename;

    bool compressed;
    uint32_t decompressedSize;
    uint32_t packedSize;

    uint32_t offset;

public:
    virtual ~DatEntry() = default;

    std::string getFilename() const;
    void setFilename(const std::string& value);

    bool getCompressed() const;
    void setCompressed(bool value);

    uint32_t getDecompressedSize() const;
    void setDecompressedSize(const uint32_t& value);

    uint32_t getPackedSize() const;
    void setPackedSize(const uint32_t& value);

    uint32_t getOffset() const;
    void setOffset(const uint32_t& value);
};

}  // namespace geck
