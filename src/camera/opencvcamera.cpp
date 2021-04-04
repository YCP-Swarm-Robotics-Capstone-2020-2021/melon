#include "opencvcamera.h"
#include <charconv>

OpenCvCamera::OpenCvCamera(StateVariables& state) : AbstractCamera(state)
{
}

bool OpenCvCamera::do_connect()
{
    // Get a local variable reference since we'll be using this string in several places
    const std::string& source = get_source();
    // Attempt to convert the string to an integer
    int device_id;
    const auto result = std::from_chars(source.data(), source.data() + source.size(), device_id);

    // If there is no error and at no point a non-integer character was encountered, use the integer value
    if(result.ec == std::errc() && (result.ptr == (source.data() + source.size())))
        return m_video_feed.open(device_id);
    // Otherwise connect with the string
    else
        return m_video_feed.open(source);
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