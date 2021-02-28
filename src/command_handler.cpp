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

std::vector<std::string> command_handler::tokenize_values(std::string values){
    std::vector<std::string> tokens;

    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = values.find(delimiter, last)) !=
           std::string::npos) {
        tokens.push_back(values.substr(last, next - last));
        last = next + 1;
    }
    // get last token after loop, since last word won't have comma after it
    tokens.push_back(values.substr(last));
    return tokens;
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
    }else if(tokens[0] == "set"){
        if(tokens.size() != 4){
            return "please provide a robot name and marker ids separated by commas\n    ex: set robot robot_1 1,2,3,4";
        }

        std::string robot_id = tokens[2];
        //tokenize by commas
        std::vector<std::string> values = tokenize_values(tokens[3]);
        //convert string values to ints
        std::vector<int> values_as_int;
        for(int i = 0; i < values.size(); i++){
            try{
                values_as_int.push_back(std::stoi(values[i]));
            }catch(const std::invalid_argument& err){
                //if user gives a list containing a non int value, let them know
                return "please provide a comma separated list of integers for '"+robot_id+"'s marker ids";
            }
        }

        current_state->robots.insert(std::pair<std::string, std::vector<int>>(robot_id, values_as_int));
        return robot_id+" added with marker values "+tokens[3];
    }else if(tokens[0] == "delete"){
        if(tokens.size() != 3){
            return "please provide a robot to delete\n    ex: delete robot robot_1";
        }

        //get count of robots before attempting delete
        int initial_num_robots = current_state->robots.size();
        std::string robot_to_delete = tokens[2];

        //remove given robot, if size didn't decrease, robot didn't exist
        current_state->robots.erase(robot_to_delete);
        if(current_state->robots.size() < initial_num_robots){
            return "robot '"+robot_to_delete+"' has been removed";
        }else{
            return "robot '"+robot_to_delete+"' does not exist";
        }
    }else{
        return "command not yet implemented for target system";
    }
}
