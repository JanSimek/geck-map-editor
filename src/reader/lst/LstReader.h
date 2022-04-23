#pragma once

#include <istream>
#include <string>

#include "../FileParser.h"

namespace geck {

class Lst;

class LstReader : public FileParser<Lst> {
public:
    std::unique_ptr<Lst> read() override;
};

} // namespace geck
