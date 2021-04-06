#ifndef MELON_SERVER_H
#define MELON_SERVER_H

#include <asio.hpp>
#include <memory>
#include "session.h"
#include "globalstate.h"

using asio::ip::tcp;
/** @brief TCP server
 *
 * @see session
 */
class server{
public:
    /** Create new TCP server instance
     *
     * @param io_context [in] IO context to run on
     * @param port [in] Port for server to run on
     * @param state [in, out] Global state of the program to read from and write to
     */
    server(asio::io_context& io_context, short port, std::shared_ptr<GlobalState> state);

private:
    /** @brief Accept new connection
     *
     * This accepts a new connection and makes a shared pointer to a new session instance. <br>
     * This also then starts the session to begin reading/writing from/to the TCP stream
     */
    void do_accept();

    tcp::acceptor acceptor_;
    std::shared_ptr<GlobalState> m_global_state;
};

#endif //MELON_SERVER_H
