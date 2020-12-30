#pragma once

#include <istream>
#include <string>

#include "../FileReader.h"

namespace geck {

class Lst;

class LstReader : FileReader<Lst> {
public:
    using FileReader::read;  // shadowed method
    std::unique_ptr<Lst> read(std::istream& stream) override;
};

}  // namespace geck
