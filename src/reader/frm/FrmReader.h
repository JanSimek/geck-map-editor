#pragma once

#include <istream>
#include <string>

#include "../FileReader.h"

namespace geck {

class Frm;

class FrmReader : FileReader<Frm> {
public:
    using FileReader::read;  // shadowed method
    std::unique_ptr<Frm> read(std::istream& stream) override;
};

}  // namespace geck
