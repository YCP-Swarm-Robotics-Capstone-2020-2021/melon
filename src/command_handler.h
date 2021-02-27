//
// Created by tim on 2/27/21.
//

#ifndef MELON_COMMAND_HANDLER_H
#define MELON_COMMAND_HANDLER_H

#include <asio.hpp>
#include "StateVariables.h"

class command_handler {
public:
    static std::string do_command(std::vector<std::string> tokens, StateVariables *current_state);
private:
    static std::string robot_command(std::vector<std::string> tokens, StateVariables *current_state);
};


#endif //MELON_COMMAND_HANDLER_H
