#pragma once

class State {
public:

    // TODO: ? on_create, on_exit

    virtual void update(float dt) = 0;
    virtual void render(float dt) = 0;
};