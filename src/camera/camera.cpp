#include "camera.h"
#include "opencvcamera.h"
#include "spinnakercamera.h"

Camera AbstractCamera::GetCamera(StateVariables& state)
{
    // TODO: Read camera type from state
    std::string cam_type = "";
    if(cam_type == "opencv")
    {
        return std::make_unique<OpenCvCamera>(state);
    }
    else if(cam_type == "")
    {
        return std::make_unique<SpinnakerCamera>(state);
    }
    else
    {
        return nullptr;
    }
}

AbstractCamera::AbstractCamera(StateVariables& state)
{
}

void AbstractCamera::enable_video_output() { m_video_output = true; }
void AbstractCamera::disable_video_output() { m_video_output = false; }
bool AbstractCamera::video_output_enabled() { return m_video_output; }
void AbstractCamera::enable_video_postprocessing() { m_video_postprocessing = true; }
void AbstractCamera::disable_video_postprocessing() { m_video_postprocessing = false; }
bool AbstractCamera::video_postprocessing_enabled() { return m_video_postprocessing; }

const CameraCalib& AbstractCamera::get_camera_calib() const { return m_calib; }
const std::string& AbstractCamera::get_connection_url() const { return m_connection_url; }

void AbstractCamera::update_state(StateVariables& state)
{

}