#pragma once

#include "State.h"

namespace geck {

class ConfigurationState : public State {
public:
    ConfigurationState();

private:
    void init() override;
    void handleEvent(const sf::Event& event) override;
    void update(const float dt) override;
    void render(const float dt) override;

    bool quit() const override;
};

} // namespace geck
