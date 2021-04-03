#ifndef MELON_ABSTRACTCAMERA_H
#define MELON_ABSTRACTCAMERA_H

#include <vector>
#include <opencv2/core/mat.hpp>
#include <memory>

#include "../cmdhandler/statevariables.h"
#include "cameracalib.h"

// Abstract base class for all camera types
// update_state() must be called immediately after object creation to ensure proper initialization
class AbstractCamera : public UpdateableState
{
public:
    virtual ~AbstractCamera() = default;
    // Don't allow copying since it would mess with connections
    AbstractCamera(AbstractCamera& other) = delete;

    // Set if the camera's video feed should be available to clients
    void enable_video_output();
    void disable_video_output();
    bool video_output_enabled();
    // Set if detected objects should be drawn to video feed before displaying to clients
    void enable_video_postprocessing();
    void disable_video_postprocessing();
    bool video_postprocessing_enabled();

    const CameraCalib& get_camera_calib() const;
    const std::string& get_connection_url() const;
    const std::string& get_type() const;
    bool is_connected() const;

    // Get a frame for processing from the camera
    // Returns if frame was successfully retrieved
    virtual bool get_frame(cv::Mat& frame)=0;

    // Throws an exception if type of this camera is different than the type in the state
    void update_state(StateVariables& state) override;

protected:
    explicit AbstractCamera(StateVariables& state);
    // Establish a connection to the camera
    // Return true if connection successful, false if connection was not successful
    // This does not need to check if the camera is already connected or disconnected, that is handled elsewhere
    virtual bool do_connect()=0;
    // Disconnect from the camera
    // Return true if disconnect was successful, false if disconnect was not successful
    // This does not need to check if the camera is already connected or disconnected, that is handled elsewhere
    virtual bool do_disconnect()=0;

private:
    bool m_video_output {false};
    bool m_video_postprocessing {true};
    bool m_connected {false};

    CameraCalib m_calib;
    std::string m_connection_url;
    const std::string m_type;
};

#endif //MELON_ABSTRACTCAMERA_H
