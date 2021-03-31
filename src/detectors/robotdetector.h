#ifndef MELON_ROBOTDETECTOR_H
#define MELON_ROBOTDETECTOR_H

#include <vector>
#include "../camera/camerawrapper.h.h"
#include "../cmdhandler/statevariables.h"
#include "robotdata.h"

class RobotDetector
{
public:
    static std::vector<RobotData> detect(CameraWrapper& camera, StateVariables& state);
};


#endif //MELON_ROBOTDETECTOR_H
