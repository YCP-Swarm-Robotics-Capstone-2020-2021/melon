#include "system.h"
#include "../commands/command.h"
#include <stdexcept>

System::System(const std::string& name) : m_name(name) { }

void System::register_cmd(const NamedCommand& cmd)
{
    m_commands.emplace(cmd.get_name(), cmd);
}

void System::register_variable(const Variable& var)
{
    m_variables.emplace(var.get_name(), var);
}

std::string System::execute_cmd(const Command& cmd, StateVariables& variables)
{
    if(m_name != cmd.system)
        throw std::invalid_argument("Command for system '" + cmd.system + "' given to '" + m_name + "'");
    else if(m_commands.find(cmd.cmd) == m_commands.end())
        return "Command '" + cmd.cmd + "' is unsupported for system '" + m_name + "'";
    else if(m_variables.find(cmd.variable) == m_variables.end())
        return "Variable '" + cmd.variable + "' does not exist for system '" + m_name + "'";
    else
        return m_commands.at(cmd.cmd).execute(cmd, variables);
}