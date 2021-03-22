//
// Created by tim on 2/27/21.
//

/**
 * This 'command handler' breaks things up into 'target systems'.
 *
 * Each target system returns a string that is the server's response
 * to the given command (these responses are sent/logged in the session class, do_read function).
 *
 * Each target system has a set (or one, or none) of state variables it modifies/reads.
 * For example:
 *      The 'robot' target system handles the 'robot' state variable
 * Another example:
 *      The 'state' target system doesn't have any state variables it modifies/reads.
 *      This is because it simply saves/loads states into/out of memory.
 *
 *  NOTE:
 *      No return responses should have any new line characters at the start/end.
 */
#include "command_handler.h"

//these commands require a target system to be given alongside them
std::string target_commands[] = {"set", "get", "delete", "list", "save", "load"};

const std::string SAVE_STATE_DIR = "states/";

/**
 * Do a command by calling a specific target system depending on user command string.
 *
 * @param tokens tokenized vector version of user command
 * @param current_state server's current state struct
 * @return response to user command as string
 */
std::string command_handler::do_command(const std::vector<std::string>& tokens, StateVariables& current_state){
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
        }else if(target_system == "collector"){
            return collector_system(tokens, current_state);
        }else if(target_system == "camera"){
            return camera_system(tokens, current_state);
        }else{
            return "target system: '"+target_system+"' not found";
        }
    }else if(command == "help"){
        return help_command();
    }else{
        return "command: '"+command+"' not found";
    }
}

/**
 * Tokenize (split) a value string by commas. <br>
 * Used for when a target system takes a value parameter as a comma separated list.
 *
 * @param values value string to tokenize
 * @return a vector of strings
 */
std::vector<std::string> command_handler::tokenize_values_by_commas(const std::string& values){
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

/**
 * Modifies the 'robots' state variable. <br>
 * A robot has a name and a collection of 4 marker int ids. <br>
 * system functions: set, get, list, and delete robots
 *
 * @param tokens tokenized vector version of user command
 * @param current_state server's current state struct
 * @return response to user command as string
 */
std::string command_handler::robot_system(const std::vector<std::string>& tokens, StateVariables& current_state){
    if(tokens[0] == "list"){
        std::string response = "Current robots:";
        for(auto const& robot : current_state.robots){
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
        std::vector<std::string> values = tokenize_values_by_commas(tokens[3]);
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

        current_state.robots.insert(std::pair<std::string, std::vector<int>>(robot_id, values_as_int));
        return robot_id+" added with marker values "+tokens[3];
    }else if(tokens[0] == "get"){
        if(tokens.size() != 3){
            return "please provide a robot to get\n    ex: get robot robot_1";
        }

        std::string robot_to_get = tokens[2];
        auto index = current_state.robots.find(robot_to_get);

        if(index == current_state.robots.end()){
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
        int initial_num_robots = current_state.robots.size();
        std::string robot_to_delete = tokens[2];

        //remove given robot, if size didn't decrease, robot didn't exist
        current_state.robots.erase(robot_to_delete);
        if(current_state.robots.size() < initial_num_robots){
            return "robot '"+robot_to_delete+"' has been removed";
        }else{
            return "robot '"+robot_to_delete+"' does not exist";
        }
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

/**
 * Saves and loads current state configurations to binary files in the 'states/' directory
 *
 * @param tokens tokenized vector version of user command
 * @param current_state server's current state struct
 * @return response to user command as string
 */
std::string command_handler::state_system(const std::vector<std::string>& tokens, StateVariables& current_state){
    if(!std::filesystem::exists(SAVE_STATE_DIR)){
        std::error_code ec;
        if(!std::filesystem::create_directory(SAVE_STATE_DIR, ec)){
            spdlog::error(ec.message());
            return "Error creating save state directory";
        }
    }

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
        State state_to_save;
        for(auto const& robot : current_state.robots){
            for(auto const& marker_id : robot.second)
                (*state_to_save.mutable_robots())[robot.first].mutable_ids()->Add(marker_id);
        }

        //save "collectors" map
        for(auto const& collector : current_state.collectors){
            (*state_to_save.mutable_collectors())[collector.first] = collector.second;
        }

        std::fstream output(SAVE_STATE_DIR+save_name, std::ios::out | std::ios::trunc | std::ios::binary);
        state_to_save.SerializeToOstream(&output);

        return "current state saved as '"+save_name+"'";
    }else if(tokens[0] == "load"){
        if(tokens.size() != 3){
            return "please provide a saved state name to load\n    ex: load state config1";
        }

        std::string load_name = tokens[2];

        State state_to_load;
        std::fstream input(SAVE_STATE_DIR+load_name, std::ios::in | std::ios::binary);

        //check if the given state file even exists, if exists parse in using protobuf
        if(!input.is_open()){
            return "given state '"+load_name+"' does not exist";
        }
        state_to_load.ParseFromIstream(&input);

        //clear robots state variable, fill from loaded State instance above
        current_state.robots.clear();
        for(auto const &robot : state_to_load.robots()){
            std::vector<int> marker_ids_to_save;
            for(auto const &marker_id : robot.second.ids()){
                marker_ids_to_save.push_back(marker_id);
            }
            current_state.robots.insert(std::pair<std::string, std::vector<int>>(robot.first, marker_ids_to_save));
        }

        //clear collector state variable, fill from loaded State
        current_state.collectors.clear();
        for(auto const &collector : state_to_load.collectors()){
            current_state.collectors.insert(std::pair<std::string, std::string>(collector.first, collector.second));
        }

        input.close();
        return "current state loaded from '"+load_name+"'";
    }else if(tokens[0] == "delete"){
        if(tokens.size() != 3){
            return "please provide a saved state to delete\n    ex: delete state config1";
        }

        std::string state_to_delete = tokens[2];

        //if value is 'current', clear out current state
        //if not, attempt to delete given save state's file
        if(state_to_delete == "current"){
            current_state.robots.clear();
            current_state.collectors.clear();
            return "current state has been cleared";
        }else{
            if(std::filesystem::remove((SAVE_STATE_DIR+state_to_delete).c_str())){
                return "state '"+state_to_delete+"' has been removed";
            }else{
                return "saved state '"+state_to_delete+"' does not exist";
            }
        }
    }else if(tokens[0] == "list"){
        std::string response = "Saved states:";

        //get file names in the 'states' directory
        for (const auto & entry : std::filesystem::directory_iterator(SAVE_STATE_DIR)){
            std::string filename_string = entry.path().string();
            response += "\n    "+filename_string.substr(filename_string.find_last_of("/") + 1);
        }

        return response;
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

/**
 * Modifies the 'collectors' state variable. <br>
 * A collector has a name and an ip address associated with it, collectors receive data via udp from camera. <br>
 * system functions: set, get, list, and delete collectors
 *
 * @param tokens tokenized vector version of user command
 * @param current_state server's current state struct
 * @return response to user command as string
 */
std::string command_handler::collector_system(const std::vector<std::string>& tokens, StateVariables& current_state) {
    if(tokens[0] == "list"){
        std::string response = "Current collectors:";

        for(auto const& collector : current_state.collectors){
            response += "\n    "+collector.first+": "+collector.second;
        }

        return response;
    }else if(tokens[0] == "set"){
        if(tokens.size() != 4){
            return "please provide a collector name and an ip\n    ex: set collector gcs 127.0.0.1";
        }

        std::string collector_id = tokens[2];

        asio::error_code ec;
        asio::ip::address ip = asio::ip::make_address(tokens[3], ec);

        if(ec){
            return "please provide a valid ipv4 address";
        }

        current_state.collectors.insert(std::pair<std::string, std::string>(collector_id, ip.to_string()));
        return collector_id+" added with ip "+ip.to_string();
    }else if(tokens[0] == "get"){
        if(tokens.size() != 3){
            return "please provide a collector to get\n    ex: get collector gcs";
        }

        std::string collector_to_get = tokens[2];
        auto index = current_state.collectors.find(collector_to_get);

        if(index == current_state.collectors.end()){
            return "collector '"+collector_to_get+"' not found";
        }else{
            std::string response = collector_to_get+": "+index->second;
            return response;
        }
    }else if(tokens[0] == "delete"){
        if(tokens.size() != 3){
            return "please provide a collector to delete\n    ex: delete collector gcs";
        }

        //get count of collector before attempting delete
        int initial_num_collectors = current_state.collectors.size();
        std::string collector_to_delete = tokens[2];

        //remove given collector, if size didn't decrease, collector didn't exist
        current_state.collectors.erase(collector_to_delete);
        if(current_state.collectors.size() < initial_num_collectors){
            return "collector '"+collector_to_delete+"' has been removed";
        }else{
            return "collector '"+collector_to_delete+"' does not exist";
        }
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

std::string command_handler::camera_system(const std::vector<std::string>& tokens, StateVariables& current_state){
    if(tokens[0] == "list"){

        // list out the state variables related to the camera
        // ex: url, camera_matrix, marker_dictionary, etc.

        return "needs implemented";
    }else if(tokens[0] == "set"){

        // check which variable they want to set (would be in tokens[2])
        // ex: url, camera_matrix, marker_dictionary, etc.

        // depending on which variable it is, parse out the values from string in tokens[3] and add to current state

        return "needs implemented";
    }else if(tokens[0] == "get"){
        // check which variable they want to get (tokens[2])
        // ex: url, camera_matrix, marker_dictionary, etc.

        // depending on which variable, get values from current state and add to response string

        return "needs implemented";
    }else if(tokens[0] == "delete"){
        // check which variable they want to clear (tokens[2])
        // ex: url, camera_matrix, marker_dictionary, etc.

        // alert user of successful clearing or if they variable they gave doesn't exist

        return "needs implemented";
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

/**
 * Display available target systems/commands to user
 *
 * @return help string
 */
std::string command_handler::help_command(){
    std::string response = "current target systems:\n";
    response += "    robot, state, collector, camera\n\n";

    response += "for the 'robot' system you can use the commands:\n";
    response += "    get, set, list, delete\n";
    response += "ex: 'get robot robot_1' or 'list robot' or 'set robot robot1 1,2,3,4' or 'delete robot robot1'\n\n";

    response += "for the 'state' system you can use the commands:\n";
    response += "    save, load, delete, list\n";
    response += "ex: 'save state config1' or 'load state config1' or 'delete state config1' or 'list state'\n";
    response += "NOTE: keyword 'current' is used for clearing current state ('delete state current'). Cannot save a state with the name 'current'\n\n";

    response += "for the 'collector' system you can use the commands:\n";
    response += "    get, set, list, delete\n";
    response += "ex: 'get collector gcs' or 'list collector' or 'set collector gcs 127.0.0.1' or 'delete collector gcs'\n\n";

    response += "for the 'camera' system you can use the commands:\n";
    response += "    get, set, list, delete\n";
    response += "you can modify the following variables:\n";
    response += "    url, camera_matrix, distortion_matrix, marker_dictionary, camera_options\n";
    response += "ex: 'get camera url' or 'list camera' or 'set camera marker_dictionary 6' or 'delete camera url'\n\n";

    response += "intended usage for each target system/variable will be clarified if used incorrectly.\n\n";
    return response;
}