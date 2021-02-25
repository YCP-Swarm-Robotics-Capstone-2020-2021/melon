//
// Created by tim on 2/24/21.
//
// From the boost docs:
// https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
//
// Above link gave a "barebone" tcp server that accepts connections/echos back to client whatever they send.
//
#include <cstdlib>
#include <cstdio>
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
        std::cout << "new connection from: "+socket_.remote_endpoint().address().to_string() << std::endl;
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
                                                if(current_command == "quit"){
                                                    std::cout << "closed connection with: "+socket_.remote_endpoint().address().to_string() << std::endl;
                                                    socket_.close();
                                                }else{
                                                    std::chrono::time_point now = std::chrono::system_clock::now();
                                                    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
                                                    std::cout << socket_.remote_endpoint().address().to_string()+": "+current_command+" @ "+ctime(&now_t);

                                                    //if not 'quit', tokenize input by " "
                                                    std::vector<std::string> tokens;

                                                    std::string delimiter = " ";
                                                    size_t last = 0;
                                                    size_t next = 0;
                                                    while((next = current_command.find(delimiter, last)) != std::string::npos){
                                                        tokens.push_back(current_command.substr(last, next-last));
                                                        last = next + 1;
                                                    }
                                                    // get last token after loop, since last word won't have space after it
                                                    tokens.push_back(current_command.substr(last));

                                                    //TODO: pass to some input/token handler class
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

        //start logging
        std::chrono::time_point today = std::chrono::system_clock::now();
        std::time_t today_t = std::chrono::system_clock::to_time_t(today);
        std::string logname = std::string(ctime(&today_t))+".log";
        freopen(logname.c_str(), "w", stdout);
        std::cout << ctime(&today_t) << std::endl;

        asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

