#include "camera.h"

const std::vector<RobotData>& Camera::get_robot_data() const { return m_robot_data; }

void Camera::enable_video_output() { m_video_output = true; }
void Camera::disable_video_output() { m_video_output = false; }
bool Camera::video_output_enabled() { return m_video_output; }
void Camera::enable_video_postprocessing() { m_video_postprocessing = true; }
void Camera::disable_video_postprocessing() { m_video_postprocessing = false; }
bool Camera::video_postprocessing_enabled() { return m_video_postprocessing; }

const CameraCalib& Camera::get_camera_calib() const { return m_calib; }
const state_variables& Camera::get_state() const { return m_local_state; }

void Camera::process_frame()
{
    if(m_global_state.flag.load())
        process_state_update();

    cv::Mat frame;
    get_frame(frame);
}

void Camera::process_state_update()
{
    const std::lock_guard<std::mutex> lock(m_global_state.mutex);
    m_local_state = m_global_state.variables;
    m_global_state.flag.store(false);
}