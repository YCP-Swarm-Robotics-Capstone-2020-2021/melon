//
// Created by tim on 2/27/21.
//

#ifndef MELON_STATEVARIABLES_H
#define MELON_STATEVARIABLES_H

#include <unordered_map>
#include <opencv2/core/mat.hpp>
#include <vector>
#include <string>
#include <atomic>
#include <asio.hpp>
#include <asio/ip/udp.hpp>

struct RobotSystem
{
    std::unordered_map<std::string, std::vector<int>> robots;
};

struct CollectorSystem
{
    std::unordered_map<std::string, asio::ip::udp::endpoint> collectors;
};

struct CameraSystem
{
    std::string type;
    bool connected = false;
    std::string url;
    cv::Mat camera_matrix;
    cv::Mat distortion_matrix;
    int marker_dictionary = 0;
    std::unordered_map<std::string, bool> camera_options;
};

class Variables
{
public:
    RobotSystem robot;
    CollectorSystem collector;
    CameraSystem camera;
protected:
    Variables() = default;
};

class StateVariables : public Variables
{
public:
    StateVariables() { version.store(0); }
    StateVariables(const StateVariables& other) : Variables(other) {}
    StateVariables& operator=(const StateVariables& other)
    {
        Variables::operator=(other);
        version.store(other.version.load());
        return *this;
    }

    std::atomic_uint version;
};

class UpdateableState
{
public:
    virtual void update_state(StateVariables& state)=0;
};

#endif //MELON_STATEVARIABLES_H