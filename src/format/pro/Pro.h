#pragma once

#include <istream>

namespace geck {

class Pro {
private:
    unsigned int _objectSubtypeId;

public:
    unsigned int objectSubtypeId();
    void setObjectSubtypeId(unsigned int objectSubtypeId);
};

}  // namespace geck
