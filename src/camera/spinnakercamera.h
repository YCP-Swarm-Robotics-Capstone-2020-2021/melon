#ifndef MELON_SPINNAKERCAMERA_H
#define MELON_SPINNAKERCAMERA_H

#include "camera.h"
#include <Spinnaker.h>

class SpinnakerCamera : public Camera
{
public:
    explicit SpinnakerCamera(StateVariables& state);
    ~SpinnakerCamera();
    void disconnect() override ;
    void connect() override;

    bool get_frame(cv::Mat &frame) override;

private:
    Spinnaker::SystemPtr m_psys;
    Spinnaker::CameraPtr m_pcam;
};


#endif //MELON_SPINNAKERCAMERA_H
