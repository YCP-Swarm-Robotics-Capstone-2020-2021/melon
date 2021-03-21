#include "collectorserver.h"
#include <spdlog/spdlog.h>

CollectorServer::CollectorServer(StateVariables& state) : m_socket(m_service), m_message_count(0)
{
    m_socket.open(asio::ip::udp::v4());
    update_state(state);
}

CollectorServer::~CollectorServer()
{
    if(m_socket.is_open())
        m_socket.close();
}

void CollectorServer::send(const std::string& data)
{
    std::stringstream ss;

    ss << "{\"messageNum\": \"" << m_message_count++ << "\", \"data\": \"" << data << "\"}";
    std::string message = ss.str();

    for(auto& endpoint : m_endpoints)
    {
        asio::error_code error;
        m_socket.send_to(asio::buffer(message), endpoint, 0, error);
        if(error)
            spdlog::error("Error sending message to '%s':\n%s", endpoint.address().to_string(), error.message());
    }
}

void CollectorServer::update_state(StateVariables& state)
{
    m_endpoints.clear();
    m_endpoints.reserve(state.collectors.size());
    for(auto& pair: state.collectors)
    {
        m_endpoints.push_back(pair.second);
    }
}