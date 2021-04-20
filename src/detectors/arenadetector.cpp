#include "arenadetector.h"
#include "../camera/cameracalib.h"
#include <opencv2/opencv.hpp>

ArenaDetector::ArenaDetector(const cv::Ptr<aruco::Dictionary>& dict, const cv::Ptr<aruco::DetectorParameters>& params) :
        m_dict(dict),
        m_params(params)
{

}

void ArenaDetector::detect(cv::Mat& frame, const CameraCalib& calib, bool draw_center)
{
    if(!m_mask.empty())
    {
        cv::bitwise_and(m_mask, frame, frame);
        if(draw_center)
            cv::circle(frame, cv::Point2d(m_center_pixel), 5, cv::Scalar(0, 0, 255), -1);
        return;
    }

    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;

    aruco::detectMarkers(frame, m_dict, corners, ids, m_params);

    std::unordered_map<int, std::vector<cv::Point2f>> marker_map;
    for(int i = 0; i < ids.size(); ++i)
    {
        marker_map[ids[i]] = corners[i];
    }

    if(marker_map.find(0) != marker_map.end() &&
       marker_map.find(1) != marker_map.end() &&
       marker_map.find(2) != marker_map.end() &&
       marker_map.find(3) != marker_map.end())
    {
        std::vector<cv::Vec3d> rvecs, tvecs;
        aruco::estimatePoseSingleMarkers(corners, m_marker_length, calib.matrix, calib.dist_coeffs, rvecs, tvecs);
        // Create the mask
        std::vector<std::vector<cv::Point>> maskEdges;
        maskEdges.emplace_back();
        maskEdges[0].push_back(marker_map[0][2]);
        maskEdges[0].push_back(marker_map[1][3]);
        maskEdges[0].push_back(marker_map[2][0]);
        maskEdges[0].push_back(marker_map[3][1]);

        m_mask = cv::Mat::zeros(frame.size(), frame.type());
        cv::fillPoly(m_mask, maskEdges, cv::Scalar(255, 255, 255));

        cv::Vec3d top_left, top_right, bottom_left, bottom_right;
        for(int i = 0; i < ids.size(); ++i)
        {
            switch(ids[i])
            {
                case 0:
                    top_left = tvecs[i];
                    break;
                case 1:
                    top_right = tvecs[i];
                    break;
                case 2:
                    bottom_right = tvecs[i];
                    break;
                case 3:
                    bottom_left = tvecs[i];
                    break;
            }
        }

        double dist = top_right[0] - top_left[0];
        // TODO: Pull from command handler
        double REAL_DIST = 110.0;
        m_unit = REAL_DIST / dist;

        m_center_pixel[0] = (marker_map[0][2].x + marker_map[2][0].x)/2.0;
        m_center_pixel[1] = (marker_map[0][2].y + marker_map[2][0].y)/2.0;

        m_center_tvec[0] = (top_left[0] + bottom_right[0]) / 2.0;
        m_center_tvec[1] = (top_left[1] + bottom_right[1]) / 2.0;
    }
}