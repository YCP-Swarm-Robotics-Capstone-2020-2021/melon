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

using asio::ip::tcp;
std::string current_command;

class session
        : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket): socket_(std::move(socket)){}

    void start(){
        do_write("> ", 2);
        do_read();
    }

private:
    void do_read(){
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
                                [this, self](asio::error_code ec, std::size_t length)
                                {
                                    if (!ec)
                                    {
                                        //get command from connection, build command string until new line hit.
                                        for(int i = 0; i < length; i++){
                                            char c = data_[i];
                                            if(c == '\n' || c == '\r'){
                                                std::cout << current_command << std::endl;

                                                if(current_command.compare("quit") == 0){
                                                    std::cout << "closing connection" << std::endl;
                                                    socket_.close();
                                                }

                                                //as soon as \n or \r is found stop building command
                                                current_command = "";
                                                break;
                                            }else{
                                                current_command += c;
                                            }
                                        }
                                        do_write("> ", 2);
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
    void do_accept()
    {
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

        asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

