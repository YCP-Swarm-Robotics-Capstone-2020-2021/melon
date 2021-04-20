#ifndef MELON_NAMEDCMD_H
#define MELON_NAMEDCMD_H

#include <functional>
#include "../constants/commands.h"

class Command;
class StateVariables;
using CmdFunc = std::function<std::string(const Command&, StateVariables&)>;

class NamedCommand
{
public:
    NamedCommand() = delete;
    NamedCommand(std::string name, CmdFunc func);
    explicit NamedCommand(std::string name);

    const std::string& get_name() const;
    void set_name(const std::string& name);

    std::string execute(const Command& cmd, StateVariables& variables);
    void set_cmdfunc(const CmdFunc& func);

private:
    // Name of command
    std::string m_name;
    // Function to execute when command is called
    CmdFunc m_func;
};

#define CMD_CLASS(className, strName) \
class className##Command : public NamedCommand \
{ \
public: \
    className##Command(const CmdFunc& func) : NamedCommand(strName, func) { } \
};

CMD_CLASS(Set, SET_CMD)
CMD_CLASS(Get, GET_CMD)
CMD_CLASS(Delete, DELETE_CMD)
CMD_CLASS(List, LIST_CMD)
CMD_CLASS(Save, SAVE_CMD)
CMD_CLASS(Load, LOAD_CMD)
CMD_CLASS(Help, HELP_CMD)

#endif //MELON_NAMEDCMD_H
