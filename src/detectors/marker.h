#ifndef MELON_MARKER_H
#define MELON_MARKER_H

#include <opencv2/core/types.hpp>

struct Marker
{
    int id;
    std::vector<cv::Point2f> corners;
    // Translation and rotation vectors
    cv::Vec3d rvec, tvec;
    // "Real" position - Position in <given unit of measurement> relative to the center of the area of operation
    cv::Vec3d real_pos;
    // "Real" orientation - Orientation relative to the camera frame
    double real_ort;
};

#endif //MELON_MARKER_H
