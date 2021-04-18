#include "markerdetector.h"
#include "../camera/cameracalib.h"
#include "marker.h"

MarkerDetector::MarkerDetector(CameraCalib calib) : m_calib(calib)
{
}

/*
std::vector<Marker> MarkerDetector::detect(cv::Mat& frame, bool draw)
{
    // Detect the markers
    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<int> ids;
    cv::aruco::detectMarkers(frame, m_dictionary, corners, ids, m_parameters);

    // Get the marker rotation and translation vectors
    std::vector<cv::Vec3d> rvecs, tvecs;
    // TODO: Replace -1 with user-defined marker length
    cv::aruco::estimatePoseSingleMarkers(corners, -1, m_calib.matrix, m_calib.dist_coeffs, rvecs, tvecs);

    // Draw the markers if required
    if(draw)
    {
        cv::aruco::drawDetectedMarkers(frame, corners, ids);
        cv::aruco::drawAxis(frame, m_calib.matrix, m_calib.dist_coeffs, rvecs, tvecs, 1.0);
    }

    // Wrap the marker data into Marker struct instances
    std::vector<Marker> markers(ids.size());
    for(int i = 0; i < ids.size(); ++i)
    {
        Marker m;
        m.id = ids[i];
        m.corners = corners[i];
        m.rvec = rvecs[i];
        m.tvec = tvecs[i];

        markers.push_back(m);
    }

    return markers;
}*/

void MarkerDetector::detect(cv::Mat& frame, std::vector<int>& ids, std::vector<int>& corners, bool draw)
{
    cv::aruco::detectMarkers(frame, m_dictionary, corners, ids);

    std::sort(corners.begin(), corners.end(), [&ids](size_t a, size_t b) { return ids[a] > ids[b]; });
    std::sort(ids.begin(), ids.end(), std::greater<int>());
}