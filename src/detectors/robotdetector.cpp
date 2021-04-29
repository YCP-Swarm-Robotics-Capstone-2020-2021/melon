#include "robotdetector.h"
#include "markerdetector.h"
#include "arenadetector.h"
#include "../cmdhandler/statevariables.h"

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

std::vector<RobotData> RobotDetector::detect(const DetectionResult& markers,
                                             const PoseResult& marker_poses,
                                             const ResultMap& result_map,
                                             const ArenaDetector& arena,
                                             const StateVariables& state,
                                             bool draw)
{
    std::vector<RobotData> out(state.robot.robots.size());

    cv::Mat rotation;
    for(const auto& robot : state.robot.robots)
    {
        RobotData data;
        data.id = robot.first;

        const Result& result = result_map.at(robot.second[0]);

        data.pos = arena.adjust_tvec(result.tvec);

        cv::Rodrigues(result.rvec, rotation);
        data.ort = rotationMatrixToEulerAngles(rotation)[1] * (180.0 / CV_PI);

        out.push_back(data);
    }

    return out;
}