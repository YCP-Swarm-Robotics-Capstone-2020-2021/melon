#ifndef MELON_SESSION_H
#define MELON_SESSION_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <asio.hpp>
#include <stdio.h>
#include "state_variables.h"
#include "global_state.h"

using asio::ip::tcp;
class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, std::shared_ptr<global_state> state);

    void start();
private:
    std::string get_command(char data[], std::size_t length);
    std::vector<std::string> tokenize_command(std::string command);

    void do_read();

    void do_write(std::string msg, std::size_t length);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::shared_ptr<global_state> m_state;
};

#endif //MELON_SESSION_H
