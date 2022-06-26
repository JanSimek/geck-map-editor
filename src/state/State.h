#pragma once

namespace sf {
class Event;
} // namespace sf

namespace geck {

class State {
public:
    virtual ~State() = default;

    virtual void init() = 0;
    virtual void handleEvent(const sf::Event&) = 0;
    virtual void update(const float dt) = 0;
    virtual void render(const float dt) = 0;

    virtual void quit() { _quit = true; }
    virtual bool isRunning() const { return !_quit; }

protected:
    bool _quit = false;
};

} // namespace geck
