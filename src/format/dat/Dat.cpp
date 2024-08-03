#include <filesystem>

#include "Dat.h"
#include "DatEntry.h"

namespace geck {

std::unordered_map<std::string, std::shared_ptr<DatEntry>> const& Dat::getEntries() const {
    return this->entries;
}

void Dat::addEntry(const std::filesystem::path filename, std::shared_ptr<DatEntry> entry) {
    this->entries.emplace(filename.string(), entry);
}

} // namespace geck
