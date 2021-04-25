#include "arenadetector.h"
#include "../camera/cameracalib.h"
#include <opencv2/opencv.hpp>

bool ArenaDetector::detect(cv::Mat& frame, const DetectionResult& markers, const PoseResult& poses, bool draw_center)
{
    if(!m_mask.empty())
    {
        cv::bitwise_and(m_mask, frame, frame);
        if(draw_center)
            cv::circle(frame, cv::Point2d(m_center_pixel), 5, cv::Scalar(0, 0, 255), -1);
    }
    else
    {
        std::unordered_map<int, std::vector<cv::Point2f>> marker_map;
        for(int i = 0; i < markers.ids.size(); ++i)
        {
            marker_map[markers.ids[i]] = markers.corners[i];
        }

        if(marker_map.find(0) != marker_map.end() &&
           marker_map.find(1) != marker_map.end() &&
           marker_map.find(2) != marker_map.end() &&
           marker_map.find(3) != marker_map.end())
        {
            // Create the mask
            std::vector<std::vector<cv::Point>> mask_edges;
            mask_edges.emplace_back();
            mask_edges[0].push_back(marker_map[0][2]);
            mask_edges[0].push_back(marker_map[1][3]);
            mask_edges[0].push_back(marker_map[2][0]);
            mask_edges[0].push_back(marker_map[3][1]);

            m_mask = cv::Mat::zeros(frame.size(), frame.type());
            cv::fillPoly(m_mask, mask_edges, cv::Scalar(255, 255, 255));

            cv::Vec3d top_left, top_right, bottom_left, bottom_right;
            for(int i = 0; i < markers.ids.size(); ++i)
            {
                switch(markers.ids[i])
                {
                    case 0:
                        top_left = poses.tvecs[i];
                        break;
                    case 1:
                        top_right = poses.tvecs[i];
                        break;
                    case 2:
                        bottom_right = poses.tvecs[i];
                        break;
                    case 3:
                        bottom_left = poses.tvecs[i];
                        break;
                }
            }

            double dist = top_right[0] - top_left[0];
            m_unit = m_real_dist / dist;

            m_center_pixel[0] = (marker_map[0][2].x + marker_map[2][0].x)/2.0;
            m_center_pixel[1] = (marker_map[0][2].y + marker_map[2][0].y)/2.0;

            m_center_tvec[0] = (top_left[0] + bottom_right[0]) / 2.0;
            m_center_tvec[1] = (top_left[1] + bottom_right[1]) / 2.0;
        }
    }

    return !m_mask.empty();
}