//
// Created by tim on 2/27/21.
//

#ifndef MELON_COMMAND_HANDLER_H
#define MELON_COMMAND_HANDLER_H

#include <asio.hpp>
#include <fstream>
#include <stdio.h>
#include "statevariables.h"
#include "state.pb.h"

class command_handler {
public:
    static std::string do_command(const std::vector<std::string>& tokens, StateVariables& current_state);
private:
    static std::string robot_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string state_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string collector_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string help_command();
    static std::vector<std::string> tokenize_values(const std::string& values);
};


#endif //MELON_COMMAND_HANDLER_H
