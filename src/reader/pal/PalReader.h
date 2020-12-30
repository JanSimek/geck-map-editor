#pragma once

#include "../FileReader.h"

namespace geck {

class Pal;

class PalReader : FileReader<Pal> {
public:
    using FileReader::read;  // shadowed method
    std::unique_ptr<Pal> read(std::istream& stream) override;
};

}  // namespace geck
