#pragma once

#include "../FileParser.h"

namespace geck {

class Pro;

class ProReader : public FileParser<Pro> {
public:
    std::unique_ptr<Pro> read() override;
    std::unique_ptr<Pro> loadPro(const std::filesystem::path& dataPath, unsigned int PID);
};

} // namespace geck
