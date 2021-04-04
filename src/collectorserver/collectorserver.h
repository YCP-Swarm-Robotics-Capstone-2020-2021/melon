#ifndef MELON_COLLECTORSERVER_H
#define MELON_COLLECTORSERVER_H

#include <asio.hpp>
#include "../cmdhandler/statevariables.h"

// Server for sending data from the camera to "collectors"
// Collectors are given through the command handler system; each collector is a target ip address and port number
// that data should be sent to
class CollectorServer : public UpdateableState
{
public:
    CollectorServer(StateVariables& state);
    ~CollectorServer();
    // Send the given data to all collectors
    void send(const std::string& data);

    void update_state(StateVariables& state) override;
private:
    asio::io_service m_service;
    asio::ip::udp::socket m_socket;
    std::vector<asio::ip::udp::endpoint> m_endpoints;
    std::atomic_uint m_message_count;
};


#endif //MELON_COLLECTORSERVER_H
