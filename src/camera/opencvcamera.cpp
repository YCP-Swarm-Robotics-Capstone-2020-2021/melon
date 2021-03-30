#include "opencvcamera.h"

OpenCvCamera::OpenCvCamera(StateVariables& state) : Camera(state)
{
}

bool OpenCvCamera::connect()
{
    std::string url = ""; //TODO: Get camera url from variables
    return m_video_feed.open(url);
}

bool OpenCvCamera::disconnect()
{
    m_video_feed.release();
    // OpenCV video feed doesn't return any success/error messages on disconnect, so just return true
    return true;
}

bool OpenCvCamera::get_frame(cv::Mat& frame)
{
    return m_video_feed.grab() && m_video_feed.retrieve(frame);
}