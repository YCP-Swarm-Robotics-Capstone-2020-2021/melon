#ifndef MELON_COMMANDS_H
#define MELON_COMMANDS_H

constexpr char SET_CMD[] = "set";
constexpr char GET_CMD[] = "get";
constexpr char DELETE_CMD[] = "delete";
constexpr char LIST_CMD[] = "list";
constexpr char SAVE_CMD[] = "save";
constexpr char LOAD_CMD[] = "load";
constexpr char HELP_CMD[] = "help";

const std::string TARGET_CMDS[] = {SET_CMD, GET_CMD, DELETE_CMD, LIST_CMD, SAVE_CMD, LOAD_CMD};

#endif //MELON_COMMANDS_H
