#include "robotdetector.h"

// Calculates rotation matrix to euler angles
// https://learnopencv.com/rotation-matrix-to-euler-angles/
cv::Vec3d rotationMatrixToEulerAngles(cv::Mat &R)
{
    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }

    // NOTE: OpenCV flips Y & Z axis, so 'z' is actually 'y' and vice-versa, so here it's being corrected
    //       to normal euler angle order
    return cv::Vec3d(x, z, y);
}

std::vector<RobotData> RobotDetector::detect(MarkerDetector& markers,
                                             const ArenaDetector& arena,
                                             const std::unordered_map<std::string, std::vector<int>>& robots,
                                             bool draw)
{
    std::vector<RobotData> out;
    out.reserve(robots.size());

    cv::Mat rotation;

    for(const auto& robot : robots)
    {
        if(markers.marker_detected(robot.second[0]))
        {
            RobotData data;
            data.id = robot.first;

            const Marker& marker = markers[robot.second[0]];

            data.pos = arena.adjust_tvec(marker.tvec);

            cv::Rodrigues(marker.rvec, rotation);
            data.ort = rotationMatrixToEulerAngles(rotation)[1] * (180.0 / CV_PI);

            out.push_back(data);
        }
    }

    return out;
}