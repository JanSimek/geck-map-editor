#pragma once

#include "State.h"

class EditorState : public State {
private:

public:
    void update(float dt) override;
    void render(float dt) override;
};