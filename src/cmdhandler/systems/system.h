#ifndef MELON_SYSTEM_H
#define MELON_SYSTEM_H

#include "../variables/variable.h"
#include <unordered_map>
#include "../commands/namedcmd.h"

class System
{
public:
    System() = delete;
    explicit System(const std::string& name);

    void register_cmd(const NamedCommand& cmd);
    void register_variable(const Variable& var);

    std::string execute_cmd(const Command& cmd, StateVariables& variables);

private:
    std::string m_name;
    std::unordered_map<std::string, Variable> m_variables;
    std::unordered_map<std::string, NamedCommand> m_commands;
};

#endif //MELON_SYSTEM_H
