#include "collectorserver.h"
#include <spdlog/spdlog.h>
#include <sstream>

CollectorServer::CollectorServer(const StateVariables& state) : m_socket(m_service), m_message_count(0)
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
    // Assemble the message
    std::stringstream ss;
    ss << "{\"num\": \"" << m_message_count++ << "\", \"data\": \"" << data << "\"}";
    std::string message = ss.str();
    spdlog::info("Sending message to endpoints. Message: {}", message);

    // Send the message to each collector
    for(auto& endpoint : m_endpoints)
    {
        std::stringstream ss;
        ss << "Sending message to " << endpoint;
        spdlog::info(ss.str());
        asio::error_code error;
        m_socket.send_to(asio::buffer(message), endpoint, 0, error);
        if(error)
            spdlog::error("Error sending message to '{}':\n{}", endpoint.address().to_string(), error.message());
    }

    spdlog::info("Message sent to all endpoints");
}

void CollectorServer::update_state(const StateVariables& state)
{
    // Reset and refill the collectors
    m_endpoints.clear();
    m_endpoints.reserve(state.collector.collectors.size());
    for(auto& pair: state.collector.collectors)
    {
        m_endpoints.push_back(pair.second);
    }
}