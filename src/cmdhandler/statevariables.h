//
// Created by tim on 2/27/21.
//

#ifndef MELON_STATEVARIABLES_H
#define MELON_STATEVARIABLES_H

#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>

class Variables
{
public:
    std::unordered_map<std::string, std::vector<int>> robots;
    std::unordered_map<std::string, std::string> collectors;
protected:
    Variables() = default;
};

class StateVariables : public Variables
{
public:
    StateVariables() { version.store(0); }
    StateVariables(const StateVariables& other) : Variables(other) {}
    StateVariables& operator=(const StateVariables& other)
    {
        Variables::operator=(other);
        version.store(other.version.load());
        return *this;
    }

    std::atomic_uint version;
};

#endif //MELON_STATEVARIABLES_H
