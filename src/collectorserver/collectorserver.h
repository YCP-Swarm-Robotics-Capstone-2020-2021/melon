#ifndef MELON_COLLECTORSERVER_H
#define MELON_COLLECTORSERVER_H

#include <asio.hpp>
#include "../cmdhandler/statevariables.h"

class CollectorServer : public UpdateableState
{
public:
    CollectorServer(StateVariables& state);
    ~CollectorServer();
    void send(const std::string& data);

    void update_state(StateVariables& state) override;
private:
    asio::io_service m_service;
    asio::ip::udp::socket m_socket;
    std::vector<asio::ip::udp::endpoint> m_endpoints;
    std::atomic_uint m_message_count;
};


#endif //MELON_COLLECTORSERVER_H
