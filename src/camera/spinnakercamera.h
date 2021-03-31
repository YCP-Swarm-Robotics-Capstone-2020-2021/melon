#ifndef MELON_SPINNAKERCAMERA_H
#define MELON_SPINNAKERCAMERA_H

#include "abstractcamera.h"
#include <Spinnaker.h>

class SpinnakerCamera : public AbstractCamera
{
public:
    explicit SpinnakerCamera(StateVariables& state);
    ~SpinnakerCamera();

    bool get_frame(cv::Mat &frame) override;

protected:
    bool do_disconnect() override;
    bool do_connect() override;

private:
    Spinnaker::SystemPtr m_psys;
    Spinnaker::CameraPtr m_pcam;
};


#endif //MELON_SPINNAKERCAMERA_H
