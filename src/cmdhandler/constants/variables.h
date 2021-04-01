#ifndef MELON_VARIABLES_H
#define MELON_VARIABLES_H

namespace CameraSystemVars
{
    constexpr char TYPE[] = "type";
    constexpr char CONNECTED[] = "connected";
    constexpr char URL[] = "url";
    constexpr char CAM_MATRIX[] = "camera_matrix";
    constexpr char DIST_MATRIX[] = "distortion_matrix";
    constexpr char MARKER_DICT[] = "marker_dictionary";
    constexpr char OPTIONS[] = "camera_options";

    constexpr char TYPE_OPENCV[] = "opencv";
    constexpr char TYPE_SPINNAKER[] = "spinnaker";
    const std::array<const char*, 2> TYPES = {const_cast<char*>(TYPE_OPENCV), const_cast<char*>(TYPE_SPINNAKER)};
    constexpr int CAMERA_MATRIX_ROWS = 3;
    constexpr int DISTORTION_MATRIX_ROWS = 5;
}

namespace StateSystemVars
{
    constexpr char SAVE_DIR[] = "states/";
    constexpr char CURRENT_KEYWORD[] = "current";
}


#endif //MELON_VARIABLES_H
