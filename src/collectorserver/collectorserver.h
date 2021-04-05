#ifndef MELON_COLLECTORSERVER_H
#define MELON_COLLECTORSERVER_H

#include <asio.hpp>
#include "../cmdhandler/statevariables.h"

/** @brief Server for sending camera data to collectors
 *
 * This is a server for sending data processed from the camera to end-users specified within the collectors system. <br>
 * Collectors are given through the command handler system; each collector is a target ip address and port number
 * that data should be sent to
 *
 * @see command_handler
 */
class CollectorServer : public UpdateableState
{
public:
    /** @brief Create new server instance
     *
     * @param state [in] State to receive configuration from
     */
    CollectorServer(const StateVariables& state);
    ~CollectorServer();

    // Send the given data to all collectors
    /** @brief Send data to collectors
     *
     * This sends the given data string to all of the endpoints within the collector system
     *
     * @param data [in] Data to send
     */
    void send(const std::string& data);

    void update_state(const StateVariables& state) override;
private:
    asio::io_service m_service;
    asio::ip::udp::socket m_socket;
    std::vector<asio::ip::udp::endpoint> m_endpoints;
    std::atomic_uint m_message_count;
};


#endif //MELON_COLLECTORSERVER_H
