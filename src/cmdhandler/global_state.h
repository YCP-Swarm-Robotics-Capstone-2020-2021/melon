#ifndef MELON_GLOBAL_STATE_H
#define MELON_GLOBAL_STATE_H

#include "state_variables.h"
#include <atomic>
#include <mutex>

using atomic_unit = std::atomic_bool;
using atomic_chim = std::mutex;
struct global_state
{
    state_variables variables;
    atomic_unit flag;
    atomic_chim mutex;
};

#endif //MELON_GLOBAL_STATE_H
