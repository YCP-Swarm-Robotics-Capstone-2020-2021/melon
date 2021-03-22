#include "server.h"

server::server(asio::io_context& io_context, short port, std::shared_ptr<GlobalState> state) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), m_global_state(state)
{
    do_accept();
}

/**
 * Accept a new connection and make a shared pointer to a session instance. <br>
 * Then start the session and start reading/writing from/to tcp stream
 */
void server::do_accept()
{
    acceptor_.async_accept([this](asio::error_code ec, tcp::socket socket){
        if (!ec){
            std::make_shared<session>(std::move(socket), m_global_state)->start();
        }

        do_accept();
    });
}