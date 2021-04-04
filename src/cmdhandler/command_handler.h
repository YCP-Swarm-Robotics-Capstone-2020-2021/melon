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
     * @param current_state [in] Reference to the current state of the program
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
     * @param matrix Matrix to convert
     * @return std::string containing a comma separated list of matrix values
     */
    static std::string build_matrix_string(const cv::Mat& matrix);

    static std::string robot_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string state_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string collector_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string camera_system(const std::vector<std::string>& tokens, StateVariables& current_state);
    static std::string help_command();
};


#endif //MELON_COMMAND_HANDLER_H
