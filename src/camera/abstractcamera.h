#ifndef MELON_ABSTRACTCAMERA_H
#define MELON_ABSTRACTCAMERA_H

#include <vector>
#include <opencv2/core/mat.hpp>
#include <memory>

#include "../cmdhandler/statevariables.h"
#include "cameracalib.h"

/** @brief Abstract base class for all camera types
 *
 * @note Use CameraWrapper to create a new camera. This class cannot be initialized directly, and derived classes should
 *       not be either
 * @note update_state() must be called immediately after object creation to ensure proper initialization
 */
class AbstractCamera : public UpdateableState
{
public:
    virtual ~AbstractCamera() = default;
    // Don't allow copying since it would mess with connections
    AbstractCamera(AbstractCamera& other) = delete;

    /** @brief Enable video output
     *
     * This enables the camera's video feed being displayed
     *
     * @see AbstractCamera::disable_video_output()
     * @see AbstractCamera::video_output_enabled()
     */
    void enable_video_output();

    /** @brief Disable video output
     *
     * This disables the camera's video feed being displayed
     *
     * @see AbstractCamera::enable_video_output()
     * @see AbstractCamera::video_output_enabled()
     */
    void disable_video_output();

    /** @brief The current status of video output
     *
     * @return True if video output is enabled, false if it is not
     * @see AbstractCamera::enable_video_output()
     * @see AbstractCamera::disable_video_output()
     */
    bool video_output_enabled();

    /** @brief Enable video post processing
     *
     * This enables post processing to video output, such as drawing detected ArUco markers
     *
     * @note Video output must be enabled as well, see AbstractCamera::enable_video_output()
     */
    void enable_video_postprocessing();

    /** @brief Disable video post processing
     *
     * This disables post processing to video output, such as drawing detected ArUco markers
     */
    void disable_video_postprocessing();

    /** @brief The current status of video post processing
     *
     * @return True if video postprocessing is enabled, false if it is not
     */
    bool video_postprocessing_enabled();

    /** @brief Get the calibration parameters for the camera
     *
     * @return Calibration parameters for camera
     * @see CameraCalib
     */
    const CameraCalib& get_camera_calib() const;

    /** @brief Get the connection source for the camera
     *
     * This returns the connection source for the camera; the source of the camera's video feed
     *
     * @return Camera's connection source as string
     */
    const std::string& get_source() const;

    /** @brief Get the camera type
     *
     * This returns the type of camera that this is; it signifies which of the derived classes that this camera uses
     *
     * @return Camera type as string
     * @see CameraSystemVars::TYPES
     */
    const std::string& get_type() const;

    /** @brief Is the camera currently connected
     *
     * @return True if the camera is currently connected, false if it is not
     */
    bool is_connected() const;

    /** @brief Get a frame from the camera
     *
     * This returns the most recent frame from the camera's video feed
     *
     * @param frame [out] The frame that was retrieved as a cv::Mat
     * @return True if the frame was successfully retrieved as a cv::Mat, false otherwise
     */
    virtual bool get_frame(cv::Mat& frame)=0;

    /** @brief Update camera class members from the given state
     *
     * @param state [in] State to update class members from
     * @throws std::runtime_error if this camera type is different than the camera type in the state
     */
    void update_state(StateVariables& state) override;

protected:
    /** @brief Creates new AbstractCamera instance
     *
     * @param state [in] Reference to state that this camera should get its parameters from
     */
    explicit AbstractCamera(StateVariables& state);

    /** @brief Establish connection to the camera
     *
     * This opens a new connection to the camera source
     *
     * @note This should not check if the camera is currently connected/disconnected as that is handled elsewhere
     *
     * @return True if the connection was successful, false otherwise
     * @see AbstractCamera::do_disconnect()
     */
    virtual bool do_connect()=0;

    /** @brief Disconnect from the camera
     *
     * @note This should not check if the camera is currently connected/disconnected as that is handled elsewhere
     *
     * @return True if the disconnect was successful, false otherwise
     * @see AbstractCamera::do_connect()
     */
    virtual bool do_disconnect()=0;

private:
    bool m_video_output {false};
    bool m_video_postprocessing {true};
    bool m_connected {false};

    CameraCalib m_calib;
    std::string m_source;
    const std::string m_type;
};

#endif //MELON_ABSTRACTCAMERA_H
