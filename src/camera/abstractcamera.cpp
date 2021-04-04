#include "abstractcamera.h"
#include "opencvcamera.h"
#include "spinnakercamera.h"
#include "../cmdhandler/constants/variables.h"
#include <spdlog/spdlog.h>

AbstractCamera::AbstractCamera(StateVariables& state) : m_type(state.camera.type)
{
}

void AbstractCamera::enable_video_output() { m_video_output = true; }
void AbstractCamera::disable_video_output() { m_video_output = false; }
bool AbstractCamera::video_output_enabled() { return m_video_output; }
void AbstractCamera::enable_video_postprocessing() { m_video_postprocessing = true; }
void AbstractCamera::disable_video_postprocessing() { m_video_postprocessing = false; }
bool AbstractCamera::video_postprocessing_enabled() { return m_video_postprocessing; }

const CameraCalib& AbstractCamera::get_camera_calib() const { return m_calib; }
const std::string& AbstractCamera::get_source() const { return m_source; }
const std::string& AbstractCamera::get_type() const { return m_type; }
bool AbstractCamera::is_connected() const { return m_connected; }

void AbstractCamera::update_state(StateVariables& state)
{
    if(m_type != state.camera.type)
        throw std::runtime_error("Wrong camera type -- '" + m_type + "' != '" + state.camera.type + "'");

    if(m_source != state.camera.source)
    {
        m_source = state.camera.source;
        // If the camera was connected while the source was changed and the camera should continue to be connected,
        // reset the connection
        if(is_connected() && state.camera.connected)
        {
            do_disconnect();
            do_connect();
        }
    }

    if(m_connected && !state.camera.connected)
    {
        if(!do_disconnect())
        {
            throw std::runtime_error("Camera failed to disconnect");
        }
    }
    else if(!m_connected && state.camera.connected)
    {
        if(!do_connect())
        {
            throw std::runtime_error("Camera failed to connect");
        }
    }
    m_connected = state.camera.connected;
}