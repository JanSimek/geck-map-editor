#pragma once

#include "Application.h"

#include <string>

namespace geck {

class Panel {
public:
    virtual ~Panel() = default;

    virtual void render(float dt) = 0;

    const std::string& title() const;

protected:
    std::string _title;
};

inline const std::string& Panel::title() const {
    return _title;
}

} // namespace geck
