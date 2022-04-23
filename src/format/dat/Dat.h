#pragma once

#include <memory>
#include <string>
#include <vector>

namespace geck {

class DatEntry;

class Dat {
private:
    std::string filename;

    uint32_t dataSectionSize;
    uint32_t numFiles;
    uint32_t treeOffset;
    uint32_t treeSize;
    uint32_t fileSize;

    std::vector<std::unique_ptr<DatEntry>> entries;

public:
    virtual ~Dat() = default;

    const std::vector<std::unique_ptr<DatEntry>>& getEntries() const;
    void addEntry(std::unique_ptr<DatEntry> entry);
};

} // namespace geck
