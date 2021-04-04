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
// Active connection session for a client in the command handler server
class session : public std::enable_shared_from_this<session>
{
public:
    // Create a new session instance
    // Takes the socket for the session, and a smart pointer to the global state so that it can be updated
    session(tcp::socket socket, std::shared_ptr<GlobalState> state);

    // Start the session
    void start();
private:
    // Read the tcp stream until a new line character is present in order to build a command string
    // This returns the command obtained from the user as a string
    // data is the tcp stream to read from
    // length is the length of data
    std::string get_command(char data[], std::size_t length);
    // This splits up a command string by spaces within the string
    // This returns a vector of strings, where each index is a token
    // Command is the string to tokenize
    std::vector<std::string> tokenize_command_by_spaces(std::string command);

    void do_read();

    void do_write(std::string msg, std::size_t length);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::shared_ptr<GlobalState> m_state;
};

#endif //MELON_SESSION_H
