#ifndef MELON_VARIABLES_H
#define MELON_VARIABLES_H

namespace CameraSystemVars
{
    constexpr char TYPE[] = "type";
    constexpr char CONNECTED[] = "connected";
    constexpr char SOURCE[] = "source";
    constexpr char CAM_MATRIX[] = "camera_matrix";
    constexpr char DIST_MATRIX[] = "distortion_matrix";
    constexpr char MARKER_DICT[] = "marker_dictionary";
    constexpr char OPTIONS[] = "camera_options";
    // Physical length of the side of a marker
    // Unit doesn't matter as long as its the same as arena distance
    constexpr char MARKER_LEN[] = "marker_length";
    // Physical distance between two corners of the arena, specifically markers 0 and 1
    // This is measured from the center of the markers
    // Unit doesn't matter as long as its the same as marker length
    constexpr char ARENA_DIST[] = "arena_distance";
    constexpr char VIDEO_OUTPUT[] = "video_output";
    constexpr char POSTPROCESSING[] = "postprocessing";

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
