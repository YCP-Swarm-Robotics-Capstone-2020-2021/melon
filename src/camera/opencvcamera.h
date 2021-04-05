#ifndef MELON_OPENCVCAMERA_H
#define MELON_OPENCVCAMERA_H

#include <opencv2/videoio.hpp>
#include "abstractcamera.h"

/** @brief A camera that uses OpenCV's VideoCapture class
 *
 * This class is for camera's that are interfaced with using cv::VideoCapture
 */
class OpenCvCamera : public AbstractCamera
{
public:
    explicit OpenCvCamera(const StateVariables& state);

    bool get_frame(cv::Mat& frame) override;

protected:
    bool do_disconnect() override;
    bool do_connect() override;

private:
    cv::VideoCapture m_video_feed;
};


#endif //MELON_OPENCVCAMERA_H
