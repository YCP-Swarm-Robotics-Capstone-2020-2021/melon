#include "markerdetector.h"
#include "../camera/cameracalib.h"
#include "marker.h"

DetectionResult MarkerDetector::detect(cv::Mat& frame,
                                       const cv::Ptr<aruco::Dictionary>& dict,
                                       const cv::Ptr<aruco::DetectorParameters>& params,
                                       bool draw)
{
    DetectionResult result;
    result.dictionary = dict;
    result.parameters = params;
    aruco::detectMarkers(frame, result.dictionary, result.corners, result.ids, result.parameters);

/*    std::sort(result.corners.begin(),
              result.corners.end(),
              [&result](size_t a, size_t b) { return result.ids[a] > result.ids[b]; });
    std::sort(result.ids.begin(), result.ids.end(), std::greater<int>());*/

    if(draw)
        aruco::drawDetectedMarkers(frame, result.corners, result.ids);

    return result;
}

PoseResult MarkerDetector::pose(const DetectionResult& detection_result, const CameraCalib& calib, float marker_length)
{
    PoseResult result;
    result.calib = calib;
    aruco::estimatePoseSingleMarkers(detection_result.corners,
                                     marker_length,
                                     calib.matrix,
                                     calib.dist_coeffs,
                                     result.rvecs,
                                     result.tvecs);

    return result;
}

/*
void MarkerDetector::detect(cv::Mat& frame, std::vector<int>& ids, std::vector<int>& corners, bool draw)
{
    cv::aruco::detectMarkers(frame, m_dictionary, corners, ids);

    std::sort(corners.begin(), corners.end(), [&ids](size_t a, size_t b) { return ids[a] > ids[b]; });
    std::sort(ids.begin(), ids.end(), std::greater<int>());
}*/
