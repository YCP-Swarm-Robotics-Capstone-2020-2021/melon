#ifndef MELON_STATEVARIABLES_H
#define MELON_STATEVARIABLES_H

#include <unordered_map>
#include <opencv2/core/mat.hpp>
#include <vector>
#include <string>
#include <atomic>
#include <asio.hpp>
#include <asio/ip/udp.hpp>

/** @brief Robot system state
 *
 */
struct RobotSystem
{
    std::unordered_map<std::string, std::vector<int>> robots;
};

/** @brief Collector system state
 *
 */
struct CollectorSystem
{
    std::unordered_map<std::string, asio::ip::udp::endpoint> collectors;
};

/** @brief camera system state
 *
 */
struct CameraSystem
{
    std::string type;
    bool connected = false;
    std::string source;
    cv::Mat camera_matrix;
    cv::Mat distortion_matrix;
    int marker_dictionary = 0;
    std::unordered_map<std::string, bool> camera_options;
    float marker_length;
    float arena_distance;
};

/** @brief Container class for state variables
 *
 * This is a container class for state variables to be extended by StateVariables. StateVariables
 * requires a custom copy constructor because of atomic version number, so extending this class means all of the
 * variables don't need to be manually added to the copy constructor
 *
 * @see StateVariables
 */
class Variables
{
public:
    RobotSystem robot;
    CollectorSystem collector;
    CameraSystem camera;
protected:
    Variables() = default;
};

/** @brief Variables for the system state
 *
 */
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
     * @param state [in] Program state to update from
     */
    virtual void update_state(const StateVariables& state)=0;
};

#endif //MELON_STATEVARIABLES_H