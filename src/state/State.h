#pragma once

namespace sf {
    class Window;
    class RenderTarget;
    class Event;
}

class State {
public:

    virtual ~State() = default;

    virtual void init() = 0;
    virtual void handleEvent(const sf::Event&) = 0;
    virtual void update(const float &dt) = 0;
    virtual void render(const float &dt) = 0;

    virtual bool quit() const {
        return false;
    }
};
