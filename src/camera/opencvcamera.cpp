#include "opencvcamera.h"
#include <charconv>

OpenCvCamera::OpenCvCamera(StateVariables& state) : AbstractCamera(state)
{
}

bool OpenCvCamera::do_connect()
{
    // Get a local variable reference since we'll be using this string in several places
    const std::string& connection_url = get_connection_url();
    // Attempt to convert the string to an integer
    int device_id;
    const auto result = std::from_chars(connection_url.data(), connection_url.data() + connection_url.size(), device_id);

    // If there is no error and at no point a non-integer character was encountered, use the integer value
    if(result.ec == std::errc() && (result.ptr == (connection_url.data()+connection_url.size())))
        return m_video_feed.open(device_id);
    // Otherwise connect with the string
    else
        return m_video_feed.open(connection_url);
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