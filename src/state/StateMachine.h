#pragma once

#include <memory>
#include <stack>

namespace geck {

class State;

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
