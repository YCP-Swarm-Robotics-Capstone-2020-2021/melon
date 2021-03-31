#ifndef MELON_CAMERAWRAPPER_H
#define MELON_CAMERAWRAPPER_H

#include "abstractcamera.h"
#include <memory>

class CameraWrapper : public UpdateableState
{
public:
    // Throws an exception if camera type in variables is invalid
    explicit CameraWrapper(StateVariables& variables);

    // Gets the current camera instance
    // NOTE: DO NOT STORE THIS!! If this pointer is stored somewhere and the internal camera pointer changes,
    // this pointer will be invalidated!
    // NOTE: NOT THREAD SAFE
    AbstractCamera* operator->() const noexcept;

    // Calls the wrapped camera's update_state implementation
    void update_state(StateVariables& state) override;

private:
    std::unique_ptr<AbstractCamera> m_camera;
    // Throws an exception if camera type in variables is invalid
    static std::unique_ptr<AbstractCamera> new_camera(StateVariables& state);
};


#endif //MELON_CAMERAWRAPPER_H
