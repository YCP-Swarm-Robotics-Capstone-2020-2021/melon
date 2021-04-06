#ifndef MELON_COMMAND_HANDLER_H
#define MELON_COMMAND_HANDLER_H

#include <asio.hpp>
#include <fstream>
#include <stdio.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "statevariables.h"
#include "state.pb.h"

/** @brief Command handler system
 *
 * This class handles the processing of string-based commands that control the current state of the program
 */
class command_handler {
public:
    /** @brief Execute the given command
     *
     * Given a command string that has been split into tokens, this executes the given command
     *
     * @param tokens [in] Tokenized command string (split into a std::vector by space delimiter)
     * @param current_state [in] Current state of the program
     * @return Response to the command as a string
     */
    static std::string do_command(const std::vector<std::string>& tokens, StateVariables& current_state);
private:

    /** @brief Tokenize (split) a string using commas
     *
     * This tokenizes a string based on the comma character; it is used for splitting up a value that is a comma
     * separated list
     *
     * @param values [in] String to tokenize
     * @return std::vector of strings, where each index is an element of the comma separated list
     */
    static std::vector<std::string> tokenize_values_by_commas(const std::string& values);

    /** @brief Convert std::vector of doubles to a cv::Mat
     *
     * This takes a std::vector of double values and converts it into a cv::Mat
     *
     * @param values [in] std::vector of double values that compose the matrix
     * @param rows [in] Number of rows within the matrix
     * @return Matrix of values as a cv::Mat
     * @throws std::invalid_argument if a list element is a non-double
     */
    static cv::Mat values_by_comma_to_mat(const std::vector<std::string>& values, const int rows);

    /** @brief Convert a cv::Mat to a comma separated list
     *
     * This takes a cv::Mat and converts it to a comma separated list in a string (i.e. "x,x,x,x,"
     *
     * @note Only matrix values are converted, any extra data such as number of rows/columns, etc. is lost
     *
     * @param matrix [in] Matrix to convert
     * @return std::string containing a comma separated list of matrix values
     */
    static std::string build_matrix_string(const cv::Mat& matrix);

    /** @brief Modifies the robot state system
     * 
     * This modifies the state system that handles robots. A robot has a name and a collection of 4 marker int ids.
     * 
     * Applicable commands: set, get, list, delete
     * 
     * @param tokens [in] Tokenized user command as vector of strings
     * @param current_state [in] Current program state
     * @return std::string containing response to user command
     * @see RobotSystem
     */
    static std::string robot_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    
    /** @brief Handles loading and saving of program state
     * 
     * This saves and loads (serializes and deserializes) current program state to binary files
     * 
     * Applicable commands: save, load, list, delete
     * 
     * @param tokens [in] Tokenized user command as vector of strings
     * @param current_state [in] Current program state
     * @return std::string containing response to user command
     */
    static std::string state_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    
    /** @brief Modifies the collectors state system
     * 
     * This modifies the state system that handles collectors and data distribution to collectors. <br>
     * A collector is composed of a name, ip address, and port number. Collectors receive camera data via UDP packets
     * 
     * @note Applicable system commands: set, get, list, delete
     * 
     * @param tokens [in] Tokenized user command as vector of strings
     * @param current_state [in] Current program state
     * @return std::string containing response to user command
     * @see CollectorSystem
     */
    static std::string collector_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    
    /** @brief Modifies the camera state system
     * 
     * This modifies the state system that handles the camera
     * 
     * Applicable commands: set, get, list, delete
     * 
     * @param tokens [in] Tokenized user command as vector of strings
     * @param current_state [in] Current program state
     * @return std::string containing response to user command
     * @see CameraSystem
     */
    static std::string camera_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    
    /** @brief Get help message
     * 
     * This displays available target systems and commands (with examples)
     * 
     * @return std::string containing help message
     */
    static std::string help_command();
};


#endif //MELON_COMMAND_HANDLER_H
