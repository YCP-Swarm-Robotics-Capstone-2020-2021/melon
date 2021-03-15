#include "camera.h"

Camera::Camera(StateVariables& state)
{
}

void Camera::enable_video_output() { m_video_output = true; }
void Camera::disable_video_output() { m_video_output = false; }
bool Camera::video_output_enabled() { return m_video_output; }
void Camera::enable_video_postprocessing() { m_video_postprocessing = true; }
void Camera::disable_video_postprocessing() { m_video_postprocessing = false; }
bool Camera::video_postprocessing_enabled() { return m_video_postprocessing; }

const CameraCalib& Camera::get_camera_calib() const { return m_calib; }
const std::string& Camera::get_connection_url() const { return m_connection_url; }