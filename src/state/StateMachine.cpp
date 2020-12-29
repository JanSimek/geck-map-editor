#include "StateMachine.h"

namespace geck {

void StateMachine::push(std::unique_ptr<State> state, bool replace)
{
    if (replace && !_states.empty()) {
        _states.pop();
    }
    _states.push(std::move(state));

    _states.top()->init(); // TODO: should this really be in here?
}

void StateMachine::pop()
{
    if (!_states.empty())
        _states.pop();
}

State& StateMachine::top() const
{
    return *_states.top();
}

bool StateMachine::empty() const
{
    return _states.empty();
}

}
