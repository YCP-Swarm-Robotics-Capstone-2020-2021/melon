#include "abstractcamera.h"
#include "opencvcamera.h"
#include "spinnakercamera.h"
#include "../cmdhandler/constants/variables.h"

AbstractCamera::AbstractCamera(StateVariables& state) : m_type(state.camera.type)
{
    update_state(state);
}

void AbstractCamera::enable_video_output() { m_video_output = true; }
void AbstractCamera::disable_video_output() { m_video_output = false; }
bool AbstractCamera::video_output_enabled() { return m_video_output; }
void AbstractCamera::enable_video_postprocessing() { m_video_postprocessing = true; }
void AbstractCamera::disable_video_postprocessing() { m_video_postprocessing = false; }
bool AbstractCamera::video_postprocessing_enabled() { return m_video_postprocessing; }

const CameraCalib& AbstractCamera::get_camera_calib() const { return m_calib; }
const std::string& AbstractCamera::get_connection_url() const { return m_connection_url; }
const std::string& AbstractCamera::get_type() const { return m_type; }
bool AbstractCamera::is_connected() const { return m_connected; }

void AbstractCamera::update_state(StateVariables& state)
{
    if(m_type != state.camera.type)
        throw std::runtime_error("Wrong camera type -- '" + m_type + "' != '" + state.camera.type + "'");

    if(m_connection_url != state.camera.url)
    {
        m_connection_url = state.camera.url;
        // If the camera was connected while the URL was changed, reset the connection
        if(is_connected())
        {
            do_disconnect();
            do_connect();
        }
    }

    if(m_connected && !state.camera.connected)
        if(!do_disconnect())
            throw std::runtime_error("Camera failed to disconnect");
    else if(!m_connected && state.camera.connected)
        if(!do_connect())
            throw std::runtime_error("Camera filed to connect");
    m_connected = state.camera.connected;
}