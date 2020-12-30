#include "Pro.h"

namespace geck {

unsigned int Pro::objectSubtypeId() {
    return _objectSubtypeId;
}

void Pro::setObjectSubtypeId(unsigned int objectSubtypeId) {
    _objectSubtypeId = objectSubtypeId;
}

}  // namespace geck
