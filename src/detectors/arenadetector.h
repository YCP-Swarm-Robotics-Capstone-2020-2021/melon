#ifndef MELON_ARENADETECTOR_H
#define MELON_ARENADETECTOR_H

#include "../cmdhandler/statevariables.h"
#include "marker.h"
#include "../camera/camera.h"

class ArenaDetector
{
public:
    static std::array<Marker, 4> detect(AbstractCamera& camera, StateVariables& state);
};


#endif //MELON_ARENADETECTOR_H
