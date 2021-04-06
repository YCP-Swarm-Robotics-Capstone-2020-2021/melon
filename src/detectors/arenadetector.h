#ifndef MELON_ARENADETECTOR_H
#define MELON_ARENADETECTOR_H

#include "../cmdhandler/statevariables.h"
#include "marker.h"
#include "../camera/camerawrapper.h"

class ArenaDetector
{
public:
    static std::array<Marker, 4> detect(CameraWrapper& camera, StateVariables& state);
};


#endif //MELON_ARENADETECTOR_H
