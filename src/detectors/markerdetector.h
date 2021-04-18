#ifndef MELON_MARKERDETECTOR_H
#define MELON_MARKERDETECTOR_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "../camera/cameracalib.h"

class Marker;
class MarkerDetector
{
public:
    MarkerDetector(CameraCalib calib);
     void detect(cv::Mat& frame, std::vector<int>& ids, std::vector<int>& corners, bool draw = false);
private:
    CameraCalib m_calib;
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> m_parameters;
};


#endif //MELON_MARKERDETECTOR_H
