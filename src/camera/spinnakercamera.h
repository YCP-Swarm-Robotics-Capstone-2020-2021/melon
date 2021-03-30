#ifndef MELON_SPINNAKERCAMERA_H
#define MELON_SPINNAKERCAMERA_H

#include "camera.h"
#include <Spinnaker.h>

class SpinnakerCamera : public AbstractCamera
{
public:
    explicit SpinnakerCamera(StateVariables& state);
    ~SpinnakerCamera();

    bool disconnect() override;
    bool connect() override;

    bool get_frame(cv::Mat &frame) override;
private:
    Spinnaker::SystemPtr m_psys;
    Spinnaker::CameraPtr m_pcam;
};


#endif //MELON_SPINNAKERCAMERA_H
