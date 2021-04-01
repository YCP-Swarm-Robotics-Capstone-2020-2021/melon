#include "camerawrapper.h"
#include "../cmdhandler/constants/variables.h"
#include "opencvcamera.h"
#include "spinnakercamera.h"

CameraWrapper::CameraWrapper(StateVariables& state) : m_camera(new_camera(state))
{
}

AbstractCamera* CameraWrapper::operator->() const noexcept
{
    return m_camera.get();
}

void CameraWrapper::update_state(StateVariables& state)
{
    if(m_camera->get_type() != state.camera.type)
        m_camera = new_camera(state);
    else
        m_camera->update_state(state);
}

std::unique_ptr<AbstractCamera> CameraWrapper::new_camera(StateVariables& state)
{
    if(state.camera.type == CameraSystemVars::TYPE_OPENCV)
    {
        return std::make_unique<OpenCvCamera>(state);
    }
    else if(state.camera.type == CameraSystemVars::TYPE_SPINNAKER)
    {
        return std::make_unique<SpinnakerCamera>(state);
    }
    else
    {
        throw std::runtime_error("Invalid camera type '" + state.camera.type + "'");
    }
}