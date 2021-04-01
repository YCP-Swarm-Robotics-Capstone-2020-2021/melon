#include "globalstate.h"
#include <iostream>

GlobalState::GlobalState()
{
    m_state.version.store(0);
}

void GlobalState::receive(const StateVariables& state)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_state = state;
        m_state.version.store(m_state.version.load()+1);
    }

    m_cond_var.notify_all();
}

bool GlobalState::apply(StateVariables& state)
{
    if(m_state.version > state.version)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        state = m_state;
        return true;
    }
    return false;
}

StateVariables GlobalState::get_state()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_state;
}

void GlobalState::wait(const std::function<bool(const StateVariables&)>& func)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond_var.wait(lock, [=]() { return func(m_state); });
}