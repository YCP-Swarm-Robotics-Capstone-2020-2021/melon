#ifndef MELON_MARKERDETECTOR_H
#define MELON_MARKERDETECTOR_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "../camera/cameracalib.h"

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

namespace MarkerDetector
{
    static DetectionResult detect(cv::Mat& frame,
                                  const cv::Ptr<aruco::Dictionary>& dict,
                                  const cv::Ptr<aruco::DetectorParameters>& params,
                                  bool draw = false);

    static PoseResult pose(const DetectionResult& detection_result, const CameraCalib& calib, float marker_length);
};


#endif //MELON_MARKERDETECTOR_H
