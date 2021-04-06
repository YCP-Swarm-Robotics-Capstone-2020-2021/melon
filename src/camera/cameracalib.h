#ifndef MELON_CAMERACALIB_H
#define MELON_CAMERACALIB_H

#include <opencv2/core/mat.hpp>

/** @brief Camera calibration parameters
 *
 */
struct CameraCalib
{
    /// Calibration matrix
    cv::Mat matrix;
    /// Distortion coefficients
    cv::Mat dist_coeffs;
};

#endif //MELON_CAMERACALIB_H
