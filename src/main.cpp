#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

struct CameraParams
{
    cv::Mat matrix, distCoeffs;
};

cv::Ptr<cv::aruco::DetectorParameters> ARUCO_PARAMS;
cv::Ptr<cv::aruco::Dictionary> ARUCO_DICTIONARY;

// in cm
constexpr int MARKER_SIZE = 2.0;

/// Use calibration images in directory to get camera calibration matrix and distance coefficients
/// for making sure retrieved transformations are correct
struct CameraParams getCameraParams(std::string& calibrationImgsDir);
/// Process given image and return copy of image with markers and their rotations drawn on top
cv::Mat getProcessedImage(const cv::Mat& img, const CameraParams& cameraParams);
int main()
{
    ARUCO_PARAMS = cv::aruco::DetectorParameters::create();
    ARUCO_DICTIONARY = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);

    std::string calibrationImgsDir = "../calibrationImgs/";
    struct CameraParams cameraParams = getCameraParams(calibrationImgsDir);

    cv::VideoCapture inputVideo;
    inputVideo.open(2);

    while(inputVideo.grab())
    {
        cv::Mat frame;
        inputVideo.retrieve(frame);

        cv::Mat processed = getProcessedImage(frame, cameraParams);

        cv::imshow("", processed);
        if(cv::waitKey(1) == 27)
            break;
    }

    return 0;
}

struct CameraParams getCameraParams(std::string& calibrationImgsDir)
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // 3.4 is size of all black squares in charuco board
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 3.4, 2.0, dictionary);
    cv::Size imgSize = {1280, 720};

    std::vector<std::vector<int>> allCharucoIds;
    std::vector<std::vector<cv::Point2f>> allCharucoCorners;

    // Iterate over each of the calibration images in the directory
    for(auto& p : std::filesystem::directory_iterator(calibrationImgsDir))
    {
        // Read the image and detect all of the markers
        cv::Mat img = cv::imread(p.path().string());
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners;
        cv::aruco::detectMarkers(img, board->dictionary, markerCorners, markerIds, ARUCO_PARAMS);

        // Get the marker positions within the board
        std::vector<int> charucoIds;
        std::vector<cv::Point2f> charucoCorners;
        cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, img, board, charucoCorners, charucoIds);

        allCharucoIds.push_back(charucoIds);
        allCharucoCorners.push_back(charucoCorners);
    }

    // Get the camera calibration parameters
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    int calibrationFlags = 0;
    cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs, calibrationFlags);

    return CameraParams
    {
        .matrix = cameraMatrix,
        .distCoeffs = distCoeffs,
    };
}

cv::Mat getProcessedImage(const cv::Mat& img, const CameraParams& cameraParams)
{
    cv::Mat out;
    img.copyTo(out);

    // Get the markers in the image
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(img, ARUCO_DICTIONARY, corners, ids);

    // Only if there are markers present
    if(!ids.empty())
    {
        cv::aruco::drawDetectedMarkers(out, corners, ids);

        // Get the orientation of the markers
        std::vector<cv::Vec3d> rvecs, tvecs;
        cv::aruco::estimatePoseSingleMarkers(corners, MARKER_SIZE, cameraParams.matrix, cameraParams.distCoeffs, rvecs, tvecs);
        for(int i = 0; i < ids.size(); ++i)
        {
            cv::aruco::drawAxis(out, cameraParams.matrix, cameraParams.distCoeffs, rvecs[i], tvecs[i], MARKER_SIZE/2.0);
        }
    }

    return out;
}