#pragma once

#include <istream>

namespace geck {

class Pro {
private:
    unsigned int _objectSubtypeId;
    uint32_t _messageId;

public:
    unsigned int objectSubtypeId();
    void setObjectSubtypeId(unsigned int objectSubtypeId);

    uint32_t messageId() const;
    void setMessageId(uint32_t newMessageId);
};

} // namespace geck
