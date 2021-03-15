#ifndef MELON_CAMERA_H
#define MELON_CAMERA_H

#include <vector>
#include <opencv2/core/mat.hpp>

#include "../cmdhandler/statevariables.h"
#include "cameracalib.h"

class Camera
{
public:
    virtual ~Camera() { }

    virtual void connect()=0;
    virtual void disconnect()=0;
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

    // Get a frame for processing from the camera
    // Returns if frame was successfully retrieved
    virtual bool get_frame(cv::Mat& frame)=0;

protected:
    explicit Camera(StateVariables& state);

private:
    bool m_video_output {false};
    bool m_video_postprocessing {true};
    CameraCalib m_calib;
    std::string m_connection_url;
};

#endif //MELON_CAMERA_H