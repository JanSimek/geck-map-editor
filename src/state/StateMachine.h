#pragma once

#include <memory>
#include <stack>
#include "../state/State.h"

namespace geck {

class StateMachine {
private:
    std::stack<std::unique_ptr<State>> _states;

public:
    void pop();
    void push(std::unique_ptr<State> state, bool replace = false);
    State& top() const;
    bool empty() const;
};

} // namespace geck
