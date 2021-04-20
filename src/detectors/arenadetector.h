#ifndef MELON_ARENADETECTOR_H
#define MELON_ARENADETECTOR_H

#include "../cmdhandler/statevariables.h"
#include "../camera/camerawrapper.h"
#include <opencv2/aruco.hpp>

namespace aruco = cv::aruco;

class ArenaDetector : public UpdateableState
{
public:
    ArenaDetector(const cv::Ptr<aruco::Dictionary>& dict, const cv::Ptr<aruco::DetectorParameters>& params);

    void detect(cv::Mat& frame, const CameraCalib& calib, bool draw_center = false);
private:
    // Mask for cropping frame
    cv::Mat m_mask;
    // Pixel in center of frame
    cv::Vec2d m_center_pixel;
    // Translation vector for center of frame
    cv::Vec2d m_center_tvec;
    // Multiplier for converting a translation vector into the given real-world units
    double m_unit;
    double m_marker_length;

    cv::Ptr<aruco::Dictionary> m_dict;
    cv::Ptr<aruco::DetectorParameters> m_params;
};


#endif //MELON_ARENADETECTOR_H
