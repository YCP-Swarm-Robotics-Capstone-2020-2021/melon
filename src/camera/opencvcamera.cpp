#include "opencvcamera.h"

OpenCvCamera::OpenCvCamera(StateVariables& state) : Camera(state)
{
}

void OpenCvCamera::connect()
{
    m_video_feed.open(get_connection_url());
}

void OpenCvCamera::disconnect()
{
    m_video_feed.release();
}

bool OpenCvCamera::get_frame(cv::Mat& frame)
{
    return m_video_feed.grab() && m_video_feed.retrieve(frame);
}