#include "arenadetector.h"
#include "../camera/cameracalib.h"
#include <opencv2/opencv.hpp>

ArenaDetector::ArenaDetector(const StateVariables& state)
{
    update_state(state);
}

bool ArenaDetector::detected()
{
    return !m_mask.empty();
}

void ArenaDetector::apply_mask(cv::Mat& frame, bool draw_center)
{
    cv::bitwise_and(m_mask, frame, frame);
    if(draw_center)
        cv::circle(frame, cv::Point2d(m_center_pixel), 5, cv::Scalar(0, 0, 255), -1);
}

void ArenaDetector::detect(cv::Mat& frame,
                           MarkerDetector& markers)
{
    if(markers.marker_detected(0) &&
       markers.marker_detected(1) &&
       markers.marker_detected(2) &&
       markers.marker_detected(3))
    {
        // Create the mask
        std::vector<std::vector<cv::Point>> mask_edges;
        mask_edges.emplace_back();
        mask_edges[0].push_back(markers[0].corners[2]);
        mask_edges[0].push_back(markers[1].corners[3]);
        mask_edges[0].push_back(markers[2].corners[0]);
        mask_edges[0].push_back(markers[3].corners[1]);

        m_mask = cv::Mat::zeros(frame.size(), frame.type());
        cv::fillPoly(m_mask, mask_edges, cv::Scalar(255, 255, 255));

        cv::Vec3d top_left, top_right, bottom_left, bottom_right;
        for(int i = 0; i < markers.ids().size(); ++i)
        {
            switch(markers.ids()[i])
            {
                case 0:
                    top_left = markers.tvecs()[i];
                    break;
                case 1:
                    top_right = markers.tvecs()[i];
                    break;
                case 2:
                    bottom_right = markers.tvecs()[i];
                    break;
                case 3:
                    bottom_left = markers.tvecs()[i];
                    break;
            }
        }

        double dist = top_right[0] - top_left[0];
        m_unit = m_real_dist / dist;

        m_center_pixel[0] = (markers[0].corners[2].x + markers[2].corners[0].x) / 2.0;
        m_center_pixel[1] = (markers[0].corners[2].y + markers[2].corners[0].y) / 2.0;

        m_center_tvec[0] = (top_left[0] + bottom_right[0]) / 2.0;
        m_center_tvec[1] = (top_left[1] + bottom_right[1]) / 2.0;
    }
}

cv::Vec3d ArenaDetector::adjust_tvec(const cv::Vec3d& tvec) const
{
    cv::Vec3d out = tvec;
    out[0] -= m_center_tvec[0];
    out[1] -= m_center_tvec[1];
    for(int i = 0; i < 3; ++i)
    {
        out[i] *= m_unit;
    }

    return out;
}

void ArenaDetector::update_state(const StateVariables& state)
{
    // TODO: Reset mask if this changes so the mask can be recomputed
    m_real_dist = state.camera.marker_length;
}