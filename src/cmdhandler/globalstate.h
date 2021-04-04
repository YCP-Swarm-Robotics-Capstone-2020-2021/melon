#ifndef MELON_GLOBALSTATE_H
#define MELON_GLOBALSTATE_H

#include "statevariables.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

/** @brief Global state manager for program
 *
 * This wraps a StateVariables instance so that it is thread-safe and can be used to update thread-local
 * StateVariables instances
 *
 * @see StateVariables
 */
class GlobalState
{
public:
    /** @brief Create a new instance
     *
     */
    GlobalState();

    /** @brief Update the global state
     *
     * This replaces the current internal global state with the given StateVariables
     *
     * @note The version of the incoming state is ignored
     *
     * @param state [in] Const reference to the new program state
     */
    void receive(const StateVariables& state);

    /** @brief Apply the global state to a thread-local state if necessary
     *
     * This applies the global state to the given thread-local state if the global state has a newer version number
     *
     * @param state [in, out] State to be updated
     * @return If the state was updated (i.e. false if the version numbers were the same and no update was required)
     */
    bool apply(StateVariables& state);

    /** @brief Get a copy of the global state
     *
     * @return Copy of the global state
     */
    StateVariables get_state();

    // Block current thread until the condition in func returns true
    /** @brief Wait until given callback function returns true
     *
     * This blocks the thread that this function is called within until the callback function returns true.
     * The parameter in the callback function is a const reference to the global state.
     * This callback function should return true if conditions are satisfied and waiting should be finished, and false
     * otherwise.
     * The main purpose of this function is so that a thread can be blocked until the global state contains desired
     * variable values
     *
     * @note Don't forget to update any thread-local states and class instances that extent UpdateableState once
     *       waiting is finished
     *
     * @param func Callback function determining if the thread should continue waiting
     */
    void wait(const std::function<bool(const StateVariables&)>& func);
private:
    StateVariables m_state;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
};

#endif //MELON_GLOBALSTATE_H
