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

ResultMap MarkerDetector::map_results(const DetectionResult& detection_result, const PoseResult& pose_result)
{
    ResultMap map;
    for(int i = 0; i < detection_result.ids.size(); ++i)
    {
        int id = detection_result.ids[i];
        Result result(detection_result.corners[i], pose_result.rvecs[i], pose_result.tvecs[i]);
        map.emplace(id, result);
    }

    return map;
}
