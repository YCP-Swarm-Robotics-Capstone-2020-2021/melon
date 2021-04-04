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
    std::string source;
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
    StateVariables(const StateVariables& other) : Variables(other)
    {
        *this = other;
    }
    StateVariables& operator=(const StateVariables& other)
    {
        Variables::operator=(other);
        version.store(other.version.load());
        return *this;
    }

    std::atomic_uint version;
};

/** @brief Classes that can be updated alongside a state change
 *
 * This is a pure-virtual abstract base class that classes can implement to signify that their internal state
 * can change alongside a system state change (you can call ::update_state() on the object to update it instead of
 * creating a new instance)
 */
class UpdateableState
{
public:
    /** @brief Update the class instance to coincide with the program's current state
     *
     * @param state [in] Const reference to program state to update from
     */
    virtual void update_state(StateVariables& state)=0;
};

#endif //MELON_STATEVARIABLES_H