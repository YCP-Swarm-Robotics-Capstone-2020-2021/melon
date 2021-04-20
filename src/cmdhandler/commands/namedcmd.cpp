#include "namedcmd.h"

NamedCommand::NamedCommand(std::string name, CmdFunc func) : m_name(name), m_func(func) { }

NamedCommand::NamedCommand(std::string name) :
        NamedCommand(name, [](const auto&, auto&){return "unimplemented";}) { }

const std::string& NamedCommand::get_name() const { return m_name; }
void NamedCommand::set_name(const std::string& name) { m_name = name; }

std::string NamedCommand::execute(const Command& cmd, StateVariables& variables)
{
    return m_func(cmd, variables);
}
void NamedCommand::set_cmdfunc(const CmdFunc& func) { m_func = func; }