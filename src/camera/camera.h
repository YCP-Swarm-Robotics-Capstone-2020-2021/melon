#ifndef MELON_CAMERA_H
#define MELON_CAMERA_H

#include <vector>
#include <opencv2/core/mat.hpp>

#include "../cmdhandler/global_state.h"
#include "robotdata.h"
#include "cameracalib.h"

class Camera
{
public:
    const std::vector<RobotData>& get_robot_data() const;
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
    const state_variables& get_state() const;

    // Grab a new frame from the video feed and process it for data
    void process_frame();

protected:
    explicit Camera(global_state& state) : m_global_state(state) {}
    std::vector<RobotData> m_robot_data;

    // Get a frame for processing from the camera
    // Returns if frame was successfully retrieved
    virtual bool get_frame(cv::Mat& frame)=0;
private:

    void process_state_update();

    global_state& m_global_state;
    state_variables m_local_state;
    bool m_video_output {false};
    bool m_video_postprocessing {true};
    CameraCalib m_calib;
};

#endif //MELON_CAMERA_H
