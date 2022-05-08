#include "Pro.h"

namespace geck {

unsigned int Pro::objectSubtypeId() {
    return _objectSubtypeId;
}

void Pro::setObjectSubtypeId(unsigned int objectSubtypeId) {
    _objectSubtypeId = objectSubtypeId;
}

const Pro::OBJECT_TYPE Pro::type() const {
    int32_t type = (header.PID & 0x0F000000) >> 24;
    return static_cast<Pro::OBJECT_TYPE>(type);
}

} // namespace geck
