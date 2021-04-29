#ifndef MELON_ROBOTDETECTOR_H
#define MELON_ROBOTDETECTOR_H

#include <vector>
#include <opencv2/core/matx.hpp>
struct RobotData
{
    std::string id;
    cv::Vec3d pos;
    float ort;
};

class ArenaDetector;
class DetectionResult;
class PoseResult;
class Result;
class StateVariables;
class RobotDetector
{
public:
    static std::vector<RobotData> detect(const DetectionResult& markers,
                                         const PoseResult& marker_poses,
                                         const std::unordered_map<int, Result>& result_map,
                                         const ArenaDetector& arena,
                                         const StateVariables& state,
                                         bool draw = false);
};


#endif //MELON_ROBOTDETECTOR_H
