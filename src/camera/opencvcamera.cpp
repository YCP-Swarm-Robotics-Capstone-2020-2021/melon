#include "opencvcamera.h"

OpenCvCamera::OpenCvCamera(StateVariables& state) : AbstractCamera(state)
{
}

bool OpenCvCamera::do_connect()
{
    return m_video_feed.open(get_connection_url());
}

bool OpenCvCamera::do_disconnect()
{
    m_video_feed.release();
    // OpenCV video feed doesn't return any success/error messages on disconnect, so just return true
    return true;
}

bool OpenCvCamera::get_frame(cv::Mat& frame)
{
    return m_video_feed.grab() && m_video_feed.retrieve(frame);
}