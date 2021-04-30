#ifndef MELON_ROBOTDETECTOR_H
#define MELON_ROBOTDETECTOR_H

#include <vector>
#include <opencv2/core/matx.hpp>
#include <unordered_map>

#include "markerdetector.h"
#include "arenadetector.h"

struct RobotData
{
    std::string id;
    cv::Vec3d pos;
    float ort;
};

namespace RobotDetector
{
    std::vector<RobotData> detect(MarkerDetector& markers,
                                  const ArenaDetector& arena,
                                  const std::unordered_map<std::string, std::vector<int>>& robots,
                                  bool draw = false);
};


#endif //MELON_ROBOTDETECTOR_H
