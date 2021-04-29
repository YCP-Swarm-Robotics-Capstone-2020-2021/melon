#ifndef MELON_MARKERDETECTOR_H
#define MELON_MARKERDETECTOR_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "../camera/cameracalib.h"
#include <unordered_map>

namespace aruco = cv::aruco;

struct DetectionResult
{
    cv::Ptr<aruco::Dictionary> dictionary;
    cv::Ptr<aruco::DetectorParameters> parameters;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
};
struct PoseResult
{
    CameraCalib calib;
    std::vector<cv::Vec3d> rvecs, tvecs;
};
class Result
{
public:
    Result(const std::vector<cv::Point2f>& corners, const cv::Vec3d& rvec, const cv::Vec3d& tvec)
        : corners(corners), rvec(rvec), tvec(tvec){}
    const std::vector<cv::Point2f>& corners;
    const cv::Vec3d& rvec;
    const cv::Vec3d& tvec;
};
using ResultMap = std::unordered_map<int, Result>;
namespace MarkerDetector
{
    DetectionResult detect(cv::Mat& frame,
                                  const cv::Ptr<aruco::Dictionary>& dict,
                                  const cv::Ptr<aruco::DetectorParameters>& params,
                                  bool draw = false);

    PoseResult pose(const DetectionResult& detection_result, const CameraCalib& calib, float marker_length);
    ResultMap map_results(const DetectionResult& detection_result, const PoseResult& pose_result);
};


#endif //MELON_MARKERDETECTOR_H
