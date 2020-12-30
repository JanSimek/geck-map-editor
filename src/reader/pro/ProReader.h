#pragma once

#include "../FileReader.h"

namespace geck {

class Pro;

class ProReader : public FileReader<Pro> {
public:
    using FileReader::read;  // shadowed method
    std::unique_ptr<Pro> read(std::istream& stream) override;
};

}  // namespace geck
