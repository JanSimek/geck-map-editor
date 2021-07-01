#pragma once

#include <istream>
#include <string>

#include "../FileParser.h"

namespace geck {

class Frm;

class FrmReader : public FileParser<Frm> {
public:
    std::unique_ptr<Frm> read() override;
};

}  // namespace geck
