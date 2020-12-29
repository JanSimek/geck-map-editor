#include "Dat.h"
#include "DatEntry.h"

namespace geck {

std::vector<std::unique_ptr<DatEntry>>const& Dat::getEntries() const
{
    return this->entries;
}

void Dat::addEntry(std::unique_ptr<DatEntry> entry) {
    this->entries.push_back(std::move(entry));
}

}
