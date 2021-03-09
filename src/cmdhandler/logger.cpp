//
// Created by tim on 2/25/21.
//

//ctime() includes a new line character at end

#include "logger.h"

void logger::start(){
    std::time_t now = get_time();
    std::string logname = std::string(ctime(&now))+".log";
    freopen(logname.c_str(), "w", stdout);
    std::cout << ctime(&now) << std::endl;
}

void logger::log_input(asio::ip::address addr, std::string msg) {
    std::time_t now = get_time();
    std::cout << addr.to_string()+": "+msg+" @ "+ctime(&now);
    std::cout.flush();
}

void logger::log_output(asio::ip::address addr, std::string msg){
    std::time_t now = get_time();
    std::cout << "> to "+addr.to_string()+" @ "+ctime(&now)+"> "+msg+"\n";
    std::cout.flush();
}

void logger::log_connection(asio::ip::address addr){
    std::time_t now = get_time();
    std::cout << "new connection from: "+addr.to_string()+" @ "+ctime(&now);
    std::cout.flush();
}

void logger::log_disconnect(asio::ip::address addr) {
    std::time_t now = get_time();
    std::cout << "closed connection with: "+addr.to_string()+" @ "+ctime(&now);
    std::cout.flush();
}

std::time_t  logger::get_time() {
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    return (now_t);
}