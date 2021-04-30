#include "markerdetector.h"

/*DetectionResult MarkerDetector::detect(cv::Mat& frame,
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
}*/

MarkerDetector::MarkerDetector(const StateVariables& state)
{
    update_state(state);
}

void MarkerDetector::update_state(const StateVariables& state)
{
    m_dictionary = aruco::getPredefinedDictionary(state.camera.marker_dictionary);
    m_parameters = aruco::DetectorParameters::create();
    m_calib.matrix = state.camera.camera_matrix;
    m_calib.dist_coeffs = state.camera.distortion_matrix;
    m_marker_length = state.camera.marker_length;
}

int MarkerDetector::detect(cv::Mat& frame, bool draw)
{
    aruco::detectMarkers(frame, m_dictionary, m_corners, m_ids, m_parameters);

    if(draw)
        aruco::drawDetectedMarkers(frame, m_corners, m_ids);

    return m_ids.size();
}

void MarkerDetector::pose(cv::Mat& frame, bool draw, int axis_len)
{
    aruco::estimatePoseSingleMarkers(m_corners,
                                     m_marker_length,
                                     m_calib.matrix,
                                     m_calib.dist_coeffs,
                                     m_rvecs,
                                     m_tvecs);

    if(draw)
        aruco::drawAxis(frame, m_calib.matrix, m_calib.dist_coeffs, m_rvecs, m_tvecs, axis_len);
}

const cv::Ptr<aruco::Dictionary>& MarkerDetector::dictionary() const { return m_dictionary; }
const cv::Ptr<aruco::DetectorParameters>& MarkerDetector::parameters() const { return m_parameters; }
const CameraCalib& MarkerDetector::camera_calib() const { return m_calib; }
int MarkerDetector::marker_length() const { return m_marker_length; }

const std::vector<int>& MarkerDetector::ids() const { return m_ids; }
const std::vector<std::vector<cv::Point2f>>& MarkerDetector::corners() const { return m_corners; }
const std::vector<cv::Vec3d> & MarkerDetector::rvecs() const { return m_rvecs; }
const std::vector<cv::Vec3d> & MarkerDetector::tvecs() const { return m_tvecs; }

void MarkerDetector::init_marker_map()
{
    if(m_marker_map.empty())
    {
        for(int i = 0; i < m_ids.size(); ++i)
        {
            int id = m_ids[i];
            Marker marker(id, m_corners[i], m_rvecs[i], m_tvecs[i]);
            m_marker_map.emplace(id, marker);
        }
    }
}

bool MarkerDetector::marker_detected(int id)
{
    init_marker_map();
    return m_marker_map.find(id) != m_marker_map.end();
}

const Marker& MarkerDetector::get_marker(int id)
{
    init_marker_map();
    return m_marker_map.at(id);
}

const Marker & MarkerDetector::operator[](int id)
{
    return get_marker(id);
}