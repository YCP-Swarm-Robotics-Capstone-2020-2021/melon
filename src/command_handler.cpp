//
// Created by tim on 2/27/21.
//

//---no return response should have any new line characters at the start or end

#include "command_handler.h"

//these commands require a target system to be given alongside them
std::string target_commands[] = {"set", "get", "create", "delete", "list"};

std::string command_handler::do_command(std::vector<std::string> tokens, state_variables *current_state){
    std::string command = tokens[0];

    //check if command requires a target system
    if(std::find(std::begin(target_commands), std::end(target_commands), command) != std::end(target_commands)){

        //if target system not provided, return
        if(tokens.size() < 2){
            return "please provide a target system";
        }

        std::string target_system = tokens[1];

        //call corresponding target system func
        if(target_system == "robot"){
            return robot_command(tokens, current_state);
        }else{
            return "target system: '"+target_system+"' not found";
        }
    }else{
        return "command: '"+command+"' not found";
    }
}

std::string command_handler::robot_command(std::vector<std::string> tokens, state_variables *current_state){
    if(tokens[0] == "list"){
        std::string response = "Current robots:";
        for(auto const& robot : current_state->robots){
            response += "\n    "+robot.first+": ";
            for(auto const& marker_id : robot.second){
                response += std::to_string(marker_id)+",";
            }
            response.pop_back(); // remove hanging ,
        }
        return response;
    }else{
        return "command not yet implemented for target system";
    }
}
