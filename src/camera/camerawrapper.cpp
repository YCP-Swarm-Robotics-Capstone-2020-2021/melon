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

void CameraWrapper::update_state(const StateVariables& state)
{
    // If the camera type has changed, create a new camera of the new type
    if(m_camera->get_type() != state.camera.type)
        m_camera = new_camera(state);
    // Otherwise, just update the state of the camera
    else
        m_camera->update_state(state);
}

std::unique_ptr<AbstractCamera> CameraWrapper::new_camera(const StateVariables& state)
{
    std::unique_ptr<AbstractCamera> ptr;
    if(state.camera.type == CameraSystemVars::TYPE_OPENCV)
    {
        ptr = std::make_unique<OpenCvCamera>(state);
    }
    else if(state.camera.type == CameraSystemVars::TYPE_SPINNAKER)
    {
        ptr = std::make_unique<SpinnakerCamera>(state);
    }
    else
    {
        throw std::runtime_error("Invalid camera type '" + state.camera.type + "'");
    }

    ptr->update_state(state);
    return ptr;
}