#include "opencvcamera.h"

OpenCvCamera::OpenCvCamera(global_state& state) : Camera(state)
{
}

void OpenCvCamera::connect()
{
    std::string url = ""; //TODO: Get camera url from variables
    m_video_feed.open(url);
}

void OpenCvCamera::disconnect()
{
    m_video_feed.release();
}

bool OpenCvCamera::get_frame(cv::Mat& frame)
{
    return m_video_feed.grab() && m_video_feed.retrieve(frame);
}