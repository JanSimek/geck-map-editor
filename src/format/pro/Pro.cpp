#include "Pro.h"

namespace geck {

uint32_t Pro::messageId() const
{
    return _messageId;
}

void Pro::setMessageId(uint32_t newMessageId)
{
    _messageId = newMessageId;
}

unsigned int Pro::objectSubtypeId() {
    return _objectSubtypeId;
}

void Pro::setObjectSubtypeId(unsigned int objectSubtypeId) {
    _objectSubtypeId = objectSubtypeId;
}

}  // namespace geck
