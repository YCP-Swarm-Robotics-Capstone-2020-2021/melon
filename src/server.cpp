//
// Created by tim on 2/24/21.
//
// From the boost docs:
// https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
//
// Above link gave a "barebone" tcp server that accepts connections/echos back to client whatever they send.
//
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <asio.hpp>
#include <stdio.h>
#include "StateVariables.h"
#include "logger.cpp"
#include "command_handler.cpp"

using asio::ip::tcp;

std::string get_command(char data[], std::size_t length);
std::vector<std::string> tokenize_command(std::string command);
StateVariables current_state;

std::string get_command(char data[], std::size_t length){
    std::string current_command;
    for (int i = 0; i < length; i++) {
        char c = data[i];
        if (c == '\n' || c == '\r') {
            //as soon as \n or \r is found stop building command
            return current_command;
        }else{
            current_command += c;
        }
    }
}

std::vector<std::string> tokenize_command(std::string command){
    std::vector<std::string> tokens;

    std::string delimiter = " ";
    size_t last = 0;
    size_t next = 0;
    while ((next = command.find(delimiter, last)) !=
           std::string::npos) {
        tokens.push_back(command.substr(last, next - last));
        last = next + 1;
    }
    // get last token after loop, since last word won't have space after it
    tokens.push_back(command.substr(last));
    return tokens;
}

class session
        : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket): socket_(std::move(socket)){}

    void start(){
        logger::log_connection(socket_.remote_endpoint().address());
        do_write("> ", 2);
        do_read();
    }

private:
    void do_read(){
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
                                [this, self](asio::error_code ec, std::size_t length)
                                {
                                    if (!ec) {
                                        //get command from data stream
                                        std::string command = get_command(data_, length);

                                        if(command == "quit"){
                                            logger::log_disconnect(socket_.remote_endpoint().address());
                                            socket_.close();
                                        }else {
                                            logger::log_input(socket_.remote_endpoint().address(), command);

                                            //if not 'quit', tokenize input by " "
                                            std::vector<std::string> tokens = tokenize_command(command);

                                            //TODO: pass to some input/token handler class
                                            std::string response = command_handler::do_command(tokens, &current_state);
                                            logger::log_output(socket_.remote_endpoint().address(), response);
                                            do_write(response+"\n", response.length()+2);
                                        }
                                        do_write("> ", 2);
                                    }else if(ec == asio::error::eof || ec == asio::error::connection_reset){
                                        logger::log_disconnect(socket_.remote_endpoint().address());
                                    }
                                });
    }

    void do_write(std::string msg, std::size_t length){
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(msg, length),
                                 [this, self](asio::error_code ec, std::size_t length)
                                 {
                                     if (!ec)
                                     {
                                         do_read();
                                     }
                                 });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server{
public:
    server(asio::io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)){
        do_accept();
    }

private:
    void do_accept(){
        acceptor_.async_accept([this](asio::error_code ec, tcp::socket socket){
            if (!ec){
                std::make_shared<session>(std::move(socket))->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]){
    try{
        if (argc != 2){
            std::cerr << "arg1: <port>\n";
            return 1;
        }

        //start logging
        //will open file stream and redirect stdout to said file.
        logger::start();

        asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        current_state.robots.insert(std::pair<std::string, int>("robot1", 1));

        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}