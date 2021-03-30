#ifndef MELON_OPENCVCAMERA_H
#define MELON_OPENCVCAMERA_H

#include <opencv2/videoio.hpp>
#include "camera.h"
/// A camera compatible with OpenCV's "VideoCapture" class
class OpenCvCamera : public AbstractCamera
{
public:
    explicit OpenCvCamera(StateVariables& state);

    bool connect() override;
    bool disconnect() override;

    bool get_frame(cv::Mat& frame) override;

private:
    cv::VideoCapture m_video_feed;
};


#endif //MELON_OPENCVCAMERA_H
