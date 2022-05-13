#pragma once

#include "reader/FileParser.h"
#include "format/msg/Msg.h"

namespace geck {

class MsgReader : public FileParser<Msg> {
public:
    std::unique_ptr<Msg> read() override;
};

} // namespace geck
