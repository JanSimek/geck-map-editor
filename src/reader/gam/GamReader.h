#pragma once

#include "../FileParser.h"

namespace geck {

class Gam;

class GamReader : public FileParser<Gam> {
public:
    std::unique_ptr<Gam> read() override;
};

} // namespace geck
