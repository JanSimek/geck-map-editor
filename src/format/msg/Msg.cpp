#include "Msg.h"

namespace geck {

const Msg::Message& Msg::message(int id) {
    return _messages[id];
}

} // namespace geck
