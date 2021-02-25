//
// Created by tim on 2/24/21.
//
#include <iostream>
#include <asio.hpp>

using asio::ip::tcp;

int main(){
    try{
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 5000));

        while(true){
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            asio::socket_base::keep_alive option(true);
            socket.set_option(option);

            std::string msg = "hello world tcp server\n";
            asio::error_code ignored_error; //tmp ignore errors regarding comm
            asio::write(socket, asio::buffer(msg), ignored_error);
        }
    }catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
