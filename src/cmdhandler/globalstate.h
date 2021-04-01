#ifndef MELON_GLOBALSTATE_H
#define MELON_GLOBALSTATE_H

#include "statevariables.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

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

    // Block current thread until the condition in func returns true
    void wait(const std::function<bool(const StateVariables&)>& func);
private:
    StateVariables m_state;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
};

#endif //MELON_GLOBALSTATE_H
