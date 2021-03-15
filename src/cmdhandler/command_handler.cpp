//
// Created by tim on 2/27/21.
//

//---no return response should have any new line characters at the start or end

#include "command_handler.h"

//these commands require a target system to be given alongside them
std::string target_commands[] = {"set", "get", "delete", "list", "save", "load"};

std::string command_handler::do_command(std::vector<std::string> tokens, StateVariables *current_state){
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
            return robot_system(tokens, current_state);
        }else if(target_system == "state"){
            return state_system(tokens, current_state);
        }else{
            return "target system: '"+target_system+"' not found";
        }
    }else if(command == "help"){
        return help_command();
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

std::string command_handler::robot_system(std::vector<std::string> tokens, StateVariables *current_state){
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
    }else if(tokens[0] == "get"){
        if(tokens.size() != 3){
            return "please provide a robot to get\n    ex: get robot robot_1";
        }

        std::string robot_to_get = tokens[2];
        auto index = current_state->robots.find(robot_to_get);

        if(index == current_state->robots.end()){
            return "robot '"+robot_to_get+"' not found";
        }else{
            std::string response = robot_to_get+":\n    ";
            for(auto const& marker_id : index->second){
                response += std::to_string(marker_id)+",";
            }
            response.pop_back(); // remove hanging ,
            return response;
        }
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
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

std::string command_handler::state_system(std::vector<std::string> tokens, StateVariables *current_state){
    if(tokens[0] == "save"){
        if(tokens.size() != 3){
            return "please provide a name to save the current state as, or existing save to overwrite\n    ex: save state config1";
        }

        std::string save_name = tokens[2];

        //don't allow save_name to be 'current' as that is a keyword that is used in this target sys.
        if(save_name == "current"){
            return "state name '"+save_name+"' cannot be used";
        }

        //save "robots" map
        State *state_to_save = new State();
        for(auto const& robot : current_state->robots){
            for(auto const& marker_id : robot.second)
                (*state_to_save->mutable_robots())[robot.first].mutable_ids()->Add(marker_id);
        }

        std::fstream output(save_name, std::ios::out | std::ios::trunc | std::ios::binary);
        state_to_save->SerializeToOstream(&output);

        return "current state saved as '"+save_name+"'";
    }else if(tokens[0] == "load"){
        if(tokens.size() != 3){
            return "please provide a saved state name to load\n    ex: load state config1";
        }

        std::string load_name = tokens[2];

        State *state_to_load = new State();
        std::fstream input(load_name, std::ios::in | std::ios::binary);
        state_to_load->ParseFromIstream(&input);

        //clear robots state variable, fill from loaded State instance above
        current_state->robots.clear();
        for(auto const &robot : state_to_load->robots()){
            std::vector<int> marker_ids_to_save;
            for(auto const &marker_id : robot.second.ids()){
                marker_ids_to_save.push_back(marker_id);
            }
            current_state->robots.insert(std::pair<std::string, std::vector<int>>(robot.first, marker_ids_to_save));
        }

        return "current state loaded from '"+load_name+"'";
    }else if(tokens[0] == "delete"){
        if(tokens.size() != 3){
            return "please provide a saved state to delete\n    ex: delete state config1";
        }

        std::string state_to_delete = tokens[2];

        //if value is 'current', clear out current state
        //if not, attempt to delete save state's file
        if(state_to_delete == "current"){
            current_state->robots.clear();
            return "current state has been cleared";
        }else{
            if(remove(state_to_delete.c_str()) != 0){
                return "saved state '"+state_to_delete+"' does not exist";
            }else{
                return "state '"+state_to_delete+"' has been removed";
            }
        }
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

std::string command_handler::help_command(){
    std::string response = "current target systems:\n";
    response += "    robot, state\n\n";

    response += "for the 'robot' system you can use the commands:\n";
    response += "    get, set, list, delete\n";
    response += "ex: 'get robot robot_1' or 'list robot' or 'set robot robot1 1,2,3,4'\n\n";

    response += "for the 'state' system you can use the commands:\n";
    response += "    save, load, delete\n";
    response += "ex: 'save state config1' or 'load state config1' or 'delete state config1'\n\n";

    response += "intended usage for each target system will be clarified if used incorrectly.\n\n";
    return response;
}