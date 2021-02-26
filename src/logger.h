//
// Created by tim on 2/25/21.
//

#ifndef MELON_LOGGER_H
#define MELON_LOGGER_H

#include <asio.hpp>
#include <iostream>

class logger {

public:
    static std::time_t get_time();
    static void start();
    static void log_input(asio::ip::address, std::string msg);
    static void log_connection(asio::ip::address addr);
    static void log_disconnect(asio::ip::address addr);
};

#endif //MELON_LOGGER_H