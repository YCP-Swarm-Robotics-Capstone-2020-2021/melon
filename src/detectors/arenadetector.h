#ifndef MELON_ARENADETECTOR_H
#define MELON_ARENADETECTOR_H

#include "../cmdhandler/statevariables.h"
#include "../camera/camerawrapper.h"
#include "markerdetector.h"
#include <opencv2/aruco.hpp>

namespace aruco = cv::aruco;

class ArenaDetector : public UpdateableState
{
public:
    // Detect markers that make up bounds and draw mask onto frame
    bool detect(cv::Mat& frame, const DetectionResult& markers, const PoseResult& poses, bool draw_center = false);
private:
    // Mask for cropping frame
    cv::Mat m_mask;
    // Pixel in center of frame
    cv::Vec2d m_center_pixel;
    // Translation vector for center of frame
    cv::Vec2d m_center_tvec;
    // Real world distance between marker 0 and marker 1
    double m_real_dist;
    // Multiplier for converting a translation vector into the given real-world units
    double m_unit;
};


#endif //MELON_ARENADETECTOR_H
