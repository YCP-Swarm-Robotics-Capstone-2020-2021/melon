#ifndef MELON_ROBOTDATA_H
#define MELON_ROBOTDATA_H

#include <opencv2/core/matx.hpp>
struct RobotData
{
    cv::Vec3d position;
    cv::Vec3d orientation;
};

#endif //MELON_ROBOTDATA_H
