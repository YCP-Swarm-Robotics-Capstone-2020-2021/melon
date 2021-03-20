#ifndef MELON_GLOBALSTATE_H
#define MELON_GLOBALSTATE_H

#include "statevariables.h"
#include <atomic>
#include <mutex>

class GlobalState
{
public:
    GlobalState();
    // Receive an update and integrate it into the global state. The version of the incoming state is ignored
    void receive(const StateVariables& state);
    // Apply the global state to a local state, but only if necessary. Return whether
    // global state was a newer version and update was applied
    bool apply(StateVariables& state);
    // Get a new local instance of the global state
    StateVariables get_state();
private:
    StateVariables m_state;
    std::mutex m_mutex;
};

#endif //MELON_GLOBALSTATE_H
