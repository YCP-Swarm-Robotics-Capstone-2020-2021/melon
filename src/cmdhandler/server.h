#ifndef MELON_SERVER_H
#define MELON_SERVER_H

#include <asio.hpp>
#include <memory>
#include "session.h"
#include "global_state.h"

using asio::ip::tcp;
class server{
public:
    server(asio::io_context& io_context, short port, std::shared_ptr<global_state> state);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    std::shared_ptr<global_state> m_global_state;
};

#endif //MELON_SERVER_H
