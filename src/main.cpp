#include <iostream>
#include <sstream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

namespace aruco = cv::aruco;

#define GET_DICT(dict) cv::Ptr<aruco::Dictionary> dict = aruco::getPredefinedDictionary(aruco::dict);

struct CameraCalibration
{
    cv::Mat matrix, distortion;
    double error;
};
struct Markers
{
    cv::Ptr<aruco::Dictionary> dictionary;
    cv::Ptr<aruco::DetectorParameters> parameters;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners, rejectedCorners;
    std::vector<cv::Vec3d> rvecs, tvecs;
};

struct CharucoBoard
{
    std::shared_ptr<struct Markers> markers;
    cv::Ptr<aruco::CharucoBoard> board;
    std::vector<int> ids;
    std::vector<cv::Point2f> corners;
    cv::Vec3d rvec, tvec;
};

std::shared_ptr<struct Markers> detectMarkers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters);
std::shared_ptr<struct CharucoBoard> detectCharucoBoard(const cv::Mat& image, cv::Ptr<aruco::CharucoBoard> board, std::shared_ptr<struct Markers> markers);
bool estimateMarkerPoses();
bool estimateCharucoboardPose();
struct CameraCalibration calibrateCamera(const std::string& calibrationImagesDir, const cv::Ptr<aruco::CharucoBoard>& board, const cv::Ptr<aruco::DetectorParameters>& parameters);

const std::string CALIBRATION_IMAGES_DIR = "../calibrationImgs/";
const int CAMERA_ID = 2;
const int WAIT_TIME = 1;
int main()
{
    // Calibrate camera
    cv::Ptr<aruco::CharucoBoard> calibrationBoard = aruco::CharucoBoard::create(5, 7, 3.4, 2.0, aruco::getPredefinedDictionary(aruco::DICT_6X6_250));
    cv::Ptr<aruco::DetectorParameters> calibrationParams = aruco::DetectorParameters::create();
    calibrationParams->cornerRefinementMethod = aruco::CORNER_REFINE_APRILTAG;
    struct CameraCalibration calibration = calibrateCamera(CALIBRATION_IMAGES_DIR, calibrationBoard, calibrationParams);

    cv::Ptr<aruco::DetectorParameters> generalParams = aruco::DetectorParameters::create();
    GET_DICT(DICT_4X4_50);
    GET_DICT(DICT_6X6_250);

    cv::VideoCapture videoFeed;
    videoFeed.open(CAMERA_ID);
    while(videoFeed.grab())
    {
        cv::Mat frame;
        videoFeed.retrieve(frame);


        auto markers = detectMarkers(frame, DICT_6X6_250, generalParams);
        if(!markers->ids.empty())
        {

            auto board = detectCharucoBoard(frame, calibrationBoard, markers);
            if(!board->ids.empty())
            {
                aruco::estimatePoseCharucoBoard(board->corners, board->ids, calibrationBoard, calibration.matrix, calibration.distortion, board->rvec, board->tvec);

                aruco::drawDetectedCornersCharuco(frame, board->corners, board->ids, cv::Scalar(0, 0, 255));
                aruco::drawAxis(frame, calibration.matrix, calibration.distortion, board->rvec, board->tvec, 10.0f);

                for(int i = 0; i < 3; ++i)
                    board->rvec[i] *= (180.0/CV_PI);
                std::stringstream ss;
                ss << "R(x,y,z): " << board->rvec;
                cv::putText(frame, ss.str(), cv::Point(1, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);

                ss = std::stringstream();

                ss << "T(x,y,z): " << board->tvec;
                cv::putText(frame, ss.str(), cv::Point(1, 45), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
            }
        }
        cv::imshow("melon", frame);

        if(cv::waitKey(WAIT_TIME) == 27)
            break;
    }

    return 0;
}

std::shared_ptr<struct Markers> detectMarkers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters)
{
    struct Markers detectedMarkers =
            {
                .dictionary = dictionary,
                .parameters = parameters,
            };
    aruco::detectMarkers(
            image,
            dictionary,
            detectedMarkers.corners,
            detectedMarkers.ids,
            parameters,
            detectedMarkers.rejectedCorners
            );
    return std::make_shared<struct Markers>(detectedMarkers);
}

std::shared_ptr<struct CharucoBoard> detectCharucoBoard(const cv::Mat& image, cv::Ptr<aruco::CharucoBoard> board, std::shared_ptr<struct Markers> markers)
{
    struct CharucoBoard detectedBoard =
            {
                .markers = markers,
            };
    if(!markers->ids.empty())
    {
        aruco::interpolateCornersCharuco(
                markers->corners,
                markers->ids,
                image,
                board,
                detectedBoard.corners,
                detectedBoard.ids
                );
    }
    return std::make_shared<struct CharucoBoard>(detectedBoard);
}

struct CameraCalibration calibrateCamera(const std::string& calibrationImagesDir, const cv::Ptr<aruco::CharucoBoard>& board, const cv::Ptr<aruco::DetectorParameters>& parameters)
{
    std::vector<std::vector<int>> allCharucoIds;
    std::vector<std::vector<cv::Point2f>> allCharucoCorners;
    cv::Size imageSize;

    for(auto& p: std::filesystem::directory_iterator(calibrationImagesDir))
    {
        cv::Mat image = cv::imread(p.path().string());
        imageSize = image.size();

        auto markers = detectMarkers(image, board->dictionary, parameters);
        auto charucoBoard = detectCharucoBoard(image, board, markers);

        allCharucoIds.push_back(charucoBoard->ids);
        allCharucoCorners.push_back(charucoBoard->corners);
    }

    struct CameraCalibration calibration;
    std::vector<cv::Mat> rvecs, tvecs;
    int flags = 0;
    calibration.error = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, imageSize, calibration.matrix, calibration.distortion, rvecs, tvecs, flags);

    return calibration;
}