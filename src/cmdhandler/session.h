#ifndef MELON_SESSION_H
#define MELON_SESSION_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <asio.hpp>
#include <stdio.h>
#include "statevariables.h"
#include "globalstate.h"

using asio::ip::tcp;
/** @brief Command handler connection session
 *
 * This is an active connection session for a client communicating with the command handler server
 *
 * @see server
 * @see command_handler
 */
class session : public std::enable_shared_from_this<session>
{
public:
    /** @brief Create a new session instance
     *
     * @param socket [in] Socket being used for the session
     * @param state [in] Global state of the program
     */
    session(tcp::socket socket, std::shared_ptr<GlobalState> state);

    /** @brief Start the session
     *
     * This initializes a new connection. It writes "> " to the user to have a more command-line-like interface and
     * then begins reading from the connection
     *
     */
    void start();
private:
    /** @brief Read command from TCP stream
     *
     * This reads the TCP stream until a new-line character is present in order to build a command string
     *
     * @param data [in] TCP stream data
     * @param length [in] Size of TCP stream data
     * @return Command from user as std::string
     */
    std::string get_command(const char data[], std::size_t length);

    /** @brief Tokenize (split) a command string by spaces
     *
     * This splits up a command string by spaces within the string
     *
     * @param command [in] String to tokenize
     * @return std::vector of strings, where each index is a token
     */
    std::vector<std::string> tokenize_command_by_spaces(std::string command);

    /** @brief Read from the TCP stream
     *
     * This waits for and then reads data from a connection (session) instance and then returns the command handlers
     * response to the user
     *
     * @see command_handler
     * @see session::do_write()
     */
    void do_read();

    /** @brief Write to TCP stream
     *
     * This writes data to the connection (session) instance
     *
     * @param msg [in] Message string to write out
     * @param length [in] Size of message
     */
    void do_write(std::string msg, std::size_t length);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::shared_ptr<GlobalState> m_state;
};

#endif //MELON_SESSION_H
