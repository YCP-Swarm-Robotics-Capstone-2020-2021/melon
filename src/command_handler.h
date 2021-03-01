//
// Created by tim on 2/27/21.
//

#ifndef MELON_COMMAND_HANDLER_H
#define MELON_COMMAND_HANDLER_H

#include <asio.hpp>
#include <fstream>
#include "state_variables.h"
#include "state.pb.h"

class command_handler {
public:
    static std::string do_command(std::vector<std::string> tokens, state_variables *current_state);
private:
    static std::string robot_command(std::vector<std::string> tokens, state_variables *current_state);
    static std::string help_command();
    static std::string save_command(std::vector<std::string> tokens, state_variables *current_state);
    static std::string load_command(std::vector<std::string> tokens, state_variables *current_state);
    static std::vector<std::string> tokenize_values(std::string values);
};


#endif //MELON_COMMAND_HANDLER_H
