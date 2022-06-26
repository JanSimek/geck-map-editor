#include "StateMachine.h"
#include "State.h"

namespace geck {

void StateMachine::push(std::unique_ptr<State> state, bool replace) {
    if (replace && !_states.empty()) {
        _states.pop();
    }
    _states.push(std::move(state));

    _states.top()->init();
}

void StateMachine::pop() {
    if (!_states.empty())
        _states.pop();
}

State& StateMachine::top() const {
    return *_states.top();
}

bool StateMachine::empty() const {
    return _states.empty();
}

} // namespace geck
