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
#include <sstream>
#include <algorithm>

//these commands require a target system to be given alongside them
std::string target_commands[] = {"set", "get", "delete", "list", "save", "load"};

const std::string SAVE_STATE_DIR = "states/";
const int CAMERA_MATRIX_ROWS = 3;
const int DISTORTION_MATRIX_ROWS = 5;

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
 * Send a vector<string> (doubles) to a cv::Mat. <br>
 * Can throw an error if a non double value is given (std::invalid_argument)
 *
 * @param values vector<string> of values to put in matrix
 * @return matrix of values given in 'values'
 */
cv::Mat command_handler::values_by_comma_to_mat(const std::vector<std::string>& values, const int rows){
    std::vector<double> values_as_double;
    for(int i = 0; i < values.size(); i++){
        values_as_double.push_back(std::stod(values[i]));
    }
    cv::Mat return_mat (values_as_double);
    return return_mat.reshape(1,rows).clone();
}

/**
 * Build a "x,x,x,x," list based on a matrix's values
 *
 * @param matrix matrix with values to add to return string
 * @return string for server response
 */
std::string command_handler::build_matrix_string(const cv::Mat& matrix){
    std::stringstream response;

    for (int row = 0; row < matrix.rows; ++row) {
        for (int col = 0; col < matrix.cols; ++col) {
            response << matrix.at<double>(row, col);
            response << ",";
        }
    }

    return response.str();
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
        for(auto const& robot : current_state.robot.robots){
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

        current_state.robot.robots.insert(std::pair<std::string, std::vector<int>>(robot_id, values_as_int));
        return robot_id+" added with marker values "+tokens[3];
    }else if(tokens[0] == "get"){
        if(tokens.size() != 3){
            return "please provide a robot to get\n    ex: get robot robot_1";
        }

        std::string robot_to_get = tokens[2];
        auto index = current_state.robot.robots.find(robot_to_get);

        if(index == current_state.robot.robots.end()){
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
        int initial_num_robots = current_state.robot.robots.size();
        std::string robot_to_delete = tokens[2];

        //remove given robot, if size didn't decrease, robot didn't exist
        current_state.robot.robots.erase(robot_to_delete);
        if(current_state.robot.robots.size() < initial_num_robots){
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
        for(auto const& robot : current_state.robot.robots){
            for(auto const& marker_id : robot.second)
                (*state_to_save.mutable_robots())[robot.first].mutable_ids()->Add(marker_id);
        }

        //save "collectors" map
        for(auto const& collector : current_state.collector.collectors){
            Endpoint endpoint;
            endpoint.set_address(collector.second.address().to_string());
            endpoint.set_port(collector.second.port());
            (*state_to_save.mutable_collectors())[collector.first] = endpoint;
        }

        state_to_save.set_connected(current_state.camera.connected);

        //save "url" string variable
        state_to_save.set_url(current_state.camera.url);

        //save "camera_matrix" cv::Mat
        cv::Mat camera_matrix = current_state.camera.camera_matrix;
        for (int row = 0; row < camera_matrix.rows; ++row) {
            for (int col = 0; col < camera_matrix.cols; ++col) {
                state_to_save.mutable_camera_matrix()->Add(camera_matrix.at<double>(row, col));
            }
        }

        //save "distortion_matrix" cv::Mat
        cv::Mat distortion_matrix = current_state.camera.distortion_matrix;
        for (int row = 0; row < distortion_matrix.rows; ++row) {
            for (int col = 0; col < distortion_matrix.cols; ++col) {
                state_to_save.mutable_distortion_matrix()->Add(distortion_matrix.at<double>(row, col));
            }
        }

        //save marker_dictionary int
        state_to_save.set_marker_dictionary(current_state.camera.marker_dictionary);

        //save camera_options map
        for(auto const& option : current_state.camera.camera_options){
            (*state_to_save.mutable_options())[option.first] = option.second;
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
        current_state.robot.robots.clear();
        for(auto const &robot : state_to_load.robots()){
            std::vector<int> marker_ids_to_save;
            for(auto const &marker_id : robot.second.ids()){
                marker_ids_to_save.push_back(marker_id);
            }
            current_state.robot.robots.insert(std::pair<std::string, std::vector<int>>(robot.first, marker_ids_to_save));
        }

        //clear collector state variable, fill from loaded State
        current_state.collector.collectors.clear();
        for(auto const &collector : state_to_load.collectors()){
            auto endpoint = asio::ip::udp::endpoint(
                    asio::ip::make_address(collector.second.address()),
                    collector.second.port());
            current_state.collector.collectors.insert(std::pair(collector.first, endpoint));
        }

        current_state.camera.connected = state_to_load.connected();

        //fill url variable from loaded state
        current_state.camera.url = state_to_load.url();

        //clear camera_matrix state variable, fill from loaded state
        current_state.camera.camera_matrix = cv::Mat::zeros(current_state.camera.camera_matrix.size(), current_state.camera.camera_matrix.type());;
        std::vector<double> values;
        for(auto const &value : state_to_load.camera_matrix()){
            values.push_back(value);
        };
        cv::Mat new_camera_matrix (values);
        current_state.camera.camera_matrix = new_camera_matrix.reshape(1, CAMERA_MATRIX_ROWS).clone();

        //clear distortion_state variable, fill from loaded state
        current_state.camera.distortion_matrix = cv::Mat::zeros(current_state.camera.distortion_matrix.size(), current_state.camera.distortion_matrix.type());;
        values.clear();
        for(auto const &value : state_to_load.distortion_matrix()){
            values.push_back(value);
        }
        cv::Mat new_distortion_matrix (values);
        current_state.camera.distortion_matrix = new_distortion_matrix.reshape(1, DISTORTION_MATRIX_ROWS).clone();

        //fill marker_dictionary variable from loaded state
        current_state.camera.marker_dictionary = state_to_load.marker_dictionary();

        //fill camera_options map from loaded state
        current_state.camera.camera_options.clear();
        for(auto const &option : state_to_load.options()){
            current_state.camera.camera_options.insert(std::pair<std::string, bool>(option.first, option.second));
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
            current_state.robot.robots.clear();
            current_state.collector.collectors.clear();
            current_state.camera.connected = false;
            current_state.camera.url.clear();
            current_state.camera.camera_matrix = cv::Mat::zeros(current_state.camera.camera_matrix.size(), current_state.camera.camera_matrix.type());
            current_state.camera.distortion_matrix = cv::Mat::zeros(current_state.camera.distortion_matrix.size(), current_state.camera.distortion_matrix.type());
            current_state.camera.marker_dictionary = 0;
            current_state.camera.camera_options.clear();
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
        std::stringstream response;
        response << "Current collectors:";

        for(auto const& collector : current_state.collector.collectors){
            response << "\n    " + collector.first << ": " << collector.second;
        }

        return response.str();
    }else if(tokens[0] == "set"){
        if(tokens.size() != 5){
            return "please provide a collector name, ip, and port\n    ex: set collector gcs 127.0.0.1 53";
        }

        std::string collector_id = tokens[2];

        asio::error_code ec;
        asio::ip::address address = asio::ip::make_address(tokens[3], ec);

        if(ec){
            return "please provide a valid ipv4 address";
        }

        // make sure the given port is valid
        unsigned short port;
        try{
            int p = std::stoi(tokens[4]);
            if(p < 1 || p > 65535)
                return "please provide a valid port number";
            port = p;
        }catch(const std::invalid_argument& err){
            // if user gives a non-valid port, let them know
            return "please provide a valid port number";
        }

        asio::ip::udp::endpoint endpoint(address, port);

        current_state.collector.collectors.insert(std::pair(collector_id, endpoint));
        return collector_id+" added with ip "+tokens[3]+":"+tokens[4];
    }else if(tokens[0] == "get"){
        if(tokens.size() != 3){
            return "please provide a collector to get\n    ex: get collector gcs";
        }

        std::string collector_to_get = tokens[2];
        auto index = current_state.collector.collectors.find(collector_to_get);

        if(index == current_state.collector.collectors.end()){
            return "collector '"+collector_to_get+"' not found";
        }else{
            std::stringstream response;
            response << collector_to_get << ": " << index->second;
            return response.str();
        }
    }else if(tokens[0] == "delete"){
        if(tokens.size() != 3){
            return "please provide a collector to delete\n    ex: delete collector gcs";
        }

        //get count of collector before attempting delete
        int initial_num_collectors = current_state.collector.collectors.size();
        std::string collector_to_delete = tokens[2];

        //remove given collector, if size didn't decrease, collector didn't exist
        current_state.collector.collectors.erase(collector_to_delete);
        if(current_state.collector.collectors.size() < initial_num_collectors){
            return "collector '"+collector_to_delete+"' has been removed";
        }else{
            return "collector '"+collector_to_delete+"' does not exist";
        }
    }else{
        return "command '"+tokens[0]+"' not valid for target system '"+tokens[1]+"'";
    }
}

/**
 * Modifies variables in the 'CameraSystem' struct in 'Variables' class. <br>
 * system functions (on variables mentioned above): set, get, list, and delete
 *
 * @param tokens tokenized vector version of user command
 * @param current_state server's current state struct
 * @return response to user command as string
 */
std::string command_handler::camera_system(const std::vector<std::string>& tokens, StateVariables& current_state){
    if(tokens[0] == "list"){
        std::stringstream response;
        response << "Current camera variables:";

        // add connected variable
        response << "\n    connected: "+std::string(current_state.camera.connected ? "true" : "false");

        //add url variable
        response << "\n    url: "+current_state.camera.url;

        //add camera_matrix variable
        response << "\n    camera_matrix: ";
        cv::Mat camera_matrix = current_state.camera.camera_matrix;
        response << build_matrix_string(camera_matrix);

        //add distortion_matrix variable
        response << "\n    distortion_matrix: ";
        cv::Mat distortion_matrix = current_state.camera.distortion_matrix;
        response << build_matrix_string(distortion_matrix);

        //add marker_dictionary variable
        response << "\n    marker_dictionary: ";
        response << current_state.camera.marker_dictionary;

        //add camera_option variable
        response << "\n    camera_options: ";
        for(auto const &option : current_state.camera.camera_options){
            response << "\n        " << option.first << ": " << std::boolalpha << option.second;
        }

        return response.str();
    }else if(tokens[0] == "set"){
        if(tokens.size() < 3){
            return "please provide a variable to set\n    ex: set camera url http://example.com";
        }

        std::string variable = tokens[2];

        if(variable == "connected")
        {
            if(tokens.size() != 4) {
                return "please provide true or false to indicate if the camera should be connected";
            }

            std::string val = tokens[3];
            std::transform(val.begin(), val.end(), val.begin(), ::tolower);

            if(val == "true"){
                current_state.camera.connected = true;
                return "camera connected set to true";
            }
            else if(val == "false"){
                current_state.camera.connected = false;
                return "camera connected set to false";
            }
            else{
                return "please provide true or false to indicate if the camera should be connected";
            }
        }else if(variable == "url"){
            if(tokens.size() != 4){
                return "please provide a value for variable '"+variable+"'\n    ex: set camera url http://example.com";
            }
            current_state.camera.url = tokens[3];
            return "camera url set to '"+tokens[3]+"'";
        }else if(variable == "camera_matrix" || variable == "distortion_matrix"){
            // since camera_matrix/distortion_matrix are the same data type/format, just use a conditional to assign a
            // cv::Mat to the chosen variable
            if(tokens.size() != 4){
                return "please provide a comma separated list of doubles to set a matrix\n    ex: set camera "+variable+" 1926.56, 0, 1380.786, 0, 1895.6, 745.4, 0, 0, 1";
            }

            std::vector<std::string> values = tokenize_values_by_commas(tokens[3]);

            if(variable == "camera_matrix"){
                if(values.size() != 9){
                    return "please provide a comma separated list of 9 doubles, "+std::to_string(values.size())+" given";
                }

                try{
                    current_state.camera.camera_matrix = values_by_comma_to_mat(values, CAMERA_MATRIX_ROWS);
                    return "'"+variable+"' variable set with values "+tokens[3];
                }catch(const std::invalid_argument& err){
                    spdlog::error(err.what());
                    return "please provide a comma separated list of doubles";
                }
            }else{
                if(values.size() != 5){
                    return "please provide a comma separated list of 5 doubles, "+std::to_string(values.size())+" given";
                }

                try{
                    current_state.camera.distortion_matrix = values_by_comma_to_mat(values, DISTORTION_MATRIX_ROWS);
                    return "'"+variable+"' variable set with values "+tokens[3];
                }catch(const std::invalid_argument& err){
                    spdlog::error(err.what());
                    return "please provide a comma separated list of doubles";
                }
            }
        }else if(variable == "marker_dictionary"){
            if(tokens.size() != 4){
                return "please provide an integer for variable '"+variable+"'\n    ex: set camera "+variable+" 6";
            }

            try{
                current_state.camera.marker_dictionary = std::stoi(tokens[3]);
            }catch(const std::invalid_argument& err){
                spdlog::error(err.what());
                return "please provide a valid integer value";
            }

            return "'"+variable+"' variable set with value "+tokens[3];
        }else if(variable == "camera_options"){
            if(tokens.size() != 5){
                return "please provide a name and boolean value for '"+variable+"'\n    ex: set camera "+variable+" stream true";
            }

            std::string option_name = tokens[3];
            bool option_value = (tokens[4] == "true");

            //if the option already exists, update it, if not insert into camera_options map
            auto index_found = current_state.camera.camera_options.find(option_name);
            if(index_found != current_state.camera.camera_options.end()) {
                index_found->second = option_value;
            }else{
                current_state.camera.camera_options.insert(std::pair<std::string, bool>(option_name, option_value));
            }
            return "'"+option_name+"' set to "+std::to_string(option_value);
        }

        return "variable '"+variable+"' does not exist";
    }else if(tokens[0] == "get"){
        if(tokens.size() < 3){
            return "please provide a variable to get\n    ex: get camera url";
        }

        std::string variable = tokens[2];

        if(variable == "connected")
        {
            return "connected: "+std::string(current_state.camera.connected ? "true" : "false");
        }else if(variable == "url"){
            return "url: "+current_state.camera.url;
        }else if(variable == "camera_matrix" || variable == "distortion_matrix") {
            std::stringstream response;
            response << variable << ": ";

            cv::Mat matrix_to_get;
            if(variable == "camera_matrix"){
                matrix_to_get = current_state.camera.camera_matrix;
            }else if(variable == "distortion_matrix"){
                matrix_to_get = current_state.camera.distortion_matrix;
            }

            response << build_matrix_string(matrix_to_get);

            return response.str();
        }else if(variable == "marker_dictionary"){
            return "marker_dictionary: "+std::to_string(current_state.camera.marker_dictionary);
        }else if(variable == "camera_options"){
            std::stringstream response;
            response << variable << ": ";

            for(auto const &option : current_state.camera.camera_options){
                response << "\n    " << option.first << ": " << std::boolalpha << option.second;
            }

            return response.str();
        }

        return "variable '"+variable+"' does not exist";
    }else if(tokens[0] == "delete"){
        if(tokens.size() < 3){
            return "please provide a variable to delete\n    ex: delete camera url";
        }

        std::string variable = tokens[2];

        if(variable == "connected"){
            current_state.camera.connected = false;
        }else if(variable == "url"){
            current_state.camera.url = "";
        }else if(variable == "camera_matrix"){
            current_state.camera.camera_matrix = cv::Mat::zeros(current_state.camera.camera_matrix.size(), current_state.camera.camera_matrix.type());;
        }else if(variable == "distortion_matrix"){
            current_state.camera.distortion_matrix = cv::Mat::zeros(current_state.camera.distortion_matrix.size(), current_state.camera.distortion_matrix.type());;
        }else if(variable == "marker_dictionary"){
            current_state.camera.marker_dictionary = 0;
        }else if(variable == "camera_options"){
            current_state.camera.camera_options.clear();
        }else{
           return "variable '"+variable+"' does not exist";
        }

        return "'"+variable+"' variable has been deleted";
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
    response += "ex: 'get collector gcs' or 'list collector' or 'set collector gcs 127.0.0.1 53' or 'delete collector gcs'\n\n";

    response += "for the 'camera' system you can use the commands:\n";
    response += "    get, set, list (current camera variables), delete\n";
    response += "you can modify the following variables:\n";
    response += "    connected, url, camera_matrix, distortion_matrix, marker_dictionary, camera_options\n";
    response += "ex: 'get camera url' or 'list camera' or 'set camera marker_dictionary 6' or 'delete camera url'\n\n";

    response += "intended usage for each target system/variable will be clarified if used incorrectly.\n\n";
    return response;
}