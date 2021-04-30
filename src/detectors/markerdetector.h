#ifndef MELON_MARKERDETECTOR_H
#define MELON_MARKERDETECTOR_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "../camera/cameracalib.h"
#include <unordered_map>
#include "../cmdhandler/statevariables.h"

namespace aruco = cv::aruco;

class Marker
{
public:
    Marker(int id, const std::vector<cv::Point2f>& corners, const cv::Vec3d& rvec, const cv::Vec3d& tvec)
            : id(id), corners(corners), rvec(rvec), tvec(tvec){}
    int id;
    const std::vector<cv::Point2f>& corners;
    const cv::Vec3d& rvec;
    const cv::Vec3d& tvec;
};


class MarkerDetector : public UpdateableState
{
public:
    explicit MarkerDetector(const StateVariables& state);
    void update_state(const StateVariables &state) override;

    int detect(cv::Mat& frame, bool draw = false);
    void pose(cv::Mat& frame, bool draw = false, int axis_len = 2);

    const cv::Ptr<aruco::Dictionary>& dictionary() const;
    const cv::Ptr<aruco::DetectorParameters>& parameters() const;
    const CameraCalib& camera_calib() const;
    int marker_length() const;

    const std::vector<int>& ids() const;
    const std::vector<std::vector<cv::Point2f>>& corners() const;
    const std::vector<cv::Vec3d>& rvecs() const;
    const std::vector<cv::Vec3d>& tvecs() const;

    bool marker_detected(int id);
    const Marker& get_marker(int id);

    const Marker& operator[](int id);

private:
    cv::Ptr<aruco::Dictionary> m_dictionary;
    cv::Ptr<aruco::DetectorParameters> m_parameters;
    CameraCalib m_calib;
    float m_marker_length;
    std::vector<int> m_ids;
    std::vector<std::vector<cv::Point2f>> m_corners;
    std::vector<cv::Vec3d> m_rvecs, m_tvecs;

    std::unordered_map<int, Marker> m_marker_map;

    void init_marker_map();
};


#endif //MELON_MARKERDETECTOR_H
