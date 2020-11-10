#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

struct CameraParams
{
    cv::Mat matrix, distCoeffs;
};

struct Marker
{
    int id;
};

cv::Ptr<cv::aruco::DetectorParameters> ARUCO_PARAMS;
cv::Ptr<cv::aruco::Dictionary> ARUCO_DICTIONARY;

// in cm
constexpr int MARKER_SIZE = 2.0;

/// Use calibration images in directory to get camera calibration matrix and distortion coefficients
/// for making sure retrieved transformations are correct
struct CameraParams getCameraParams(std::string& calibrationImgsDir);
cv::Mat getRoi(const cv::Mat& img, const CameraParams& cameraParams);
/// Process given image and return copy of image with markers and their rotations drawn on top
cv::Mat getProcessedImage(const cv::Mat& img, const CameraParams& cameraParams);
/// https://stackoverflow.com/a/24352524/4752083
cv::Mat rotateImg(const cv::Mat& img, float angle);
cv::Mat drawBoard(const cv::Mat& img, const CameraParams& cameraParams, cv::Ptr<cv::aruco::CharucoBoard>& board);
void createBoard()
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(3, 3, 3.3f, 2.0f,/*0.04f, 0.02f,*/ dictionary);
    cv::Mat img;
    board->draw(cv::Size(2550, 3300), img, 100, 1);
    cv::imwrite("MarkerBoard.jpg", img);
}
int main()
{
    //createBoard();
    ARUCO_PARAMS = cv::aruco::DetectorParameters::create();
    ARUCO_PARAMS->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
    ARUCO_DICTIONARY = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);//cv::aruco::generateCustomDictionary(10, 6);//cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);

    std::string calibrationImgsDir = "../calibrationImgs/";
    struct CameraParams cameraParams = getCameraParams(calibrationImgsDir);

    cv::Ptr<cv::aruco::Dictionary> SixBySix_250 = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // 3.4 is size of all black squares in charuco board
    cv::Ptr<cv::aruco::CharucoBoard> charucoBoard = cv::aruco::CharucoBoard::create(5, 7, 3.4, 2.0, SixBySix_250);

    cv::Ptr<cv::aruco::Dictionary> FourByFour_100 = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    cv::Ptr<cv::aruco::CharucoBoard> charucoMarker = cv::aruco::CharucoBoard::create(3, 3, 1.91f/*0.04f*/, 0.94f/*0.02f*/, FourByFour_100);

    cv::VideoCapture inputVideo;
    inputVideo.open(2);
/*    inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, 1000);
    inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, 500);*/

    while(inputVideo.grab())
    {
        cv::Mat frame;
        inputVideo.retrieve(frame);

        cv::Mat processed = drawBoard(frame, cameraParams, charucoBoard);
//        cv::Mat processed = drawBoard(frame, cameraParams, charucoMarker);
        cv::imshow("", processed);

/*        cv::Mat processed = getProcessedImage(frame, cameraParams);
        cv::imshow("", processed);

        frame = getRoi(frame, cameraParams);
        processed = getProcessedImage(frame, cameraParams);

        cv::imshow("1", frame);*/

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

cv::Mat getRoi(const cv::Mat& img, const CameraParams& cameraParams)
{
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(img, ARUCO_DICTIONARY, corners, ids);

    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(corners, MARKER_SIZE, cameraParams.matrix, cameraParams.distCoeffs, rvecs, tvecs);

    cv::Mat out;
    img.copyTo(out);

    std::vector<std::vector<cv::Point2f>> _corners(10);

    std::vector<int> _ids(10);
    std::vector<cv::Vec3d> _rvecs(10);
    std::unordered_map<int, std::vector<cv::Point2f>> markers;
    for(int i = 0; i < ids.size(); ++i)
    {
        markers[ids[i]] = corners[i];

        _ids[ids[i]] = ids[i];
        _corners[ids[i]] = corners[i];
        _rvecs[ids[i]] = rvecs[i];
    }

    if(markers.find(0) != markers.end() && markers.find(1) != markers.end() && markers.find(3) != markers.end())
    {
        for(int i = 0; i < 3; ++i)
        {
            std::cout << _rvecs[0][i] * (180.0/CV_PI) << " ";
        }
        std::cout << std::endl;

        out = rotateImg(out, (float)_rvecs[0][1] * (180.0/CV_PI));

        float x = markers[0][2].x;
        float y = markers[0][2].y;
        float width = abs(x - markers[1][3].x);
        float height = abs(y - markers[3][0].y);

        //cv::Rect2f roi(x, y, width, height);
        //out = out(roi);
    }

    return out;
}

cv::Mat getProcessedImage(const cv::Mat& img, const CameraParams& cameraParams)
{
    cv::Mat out;
    img.copyTo(out);

    // Get the markers in the image
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(img, ARUCO_DICTIONARY, corners, ids, ARUCO_PARAMS, cv::noArray(), cameraParams.matrix, cameraParams.distCoeffs);

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

cv::Mat rotateImg(const cv::Mat& img, float angle)
{
    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((img.cols-1)/2.0, (img.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), img.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - img.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - img.rows/2.0;

    cv::Mat dst;
    cv::warpAffine(img, dst, rot, bbox.size());
    return dst;
}

float lastAngle = FLT_MAX;
cv::Mat drawBoard(const cv::Mat& img, const CameraParams& cameraParams, cv::Ptr<cv::aruco::CharucoBoard>& board)
{
    cv::Mat out;
    img.copyTo(out);

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<std::vector<cv::Point2f>> rejectedCorners;
    cv::aruco::detectMarkers(out, board->dictionary, markerCorners, markerIds, ARUCO_PARAMS, rejectedCorners, cameraParams.matrix, cameraParams.distCoeffs);
    //cv::aruco::refineDetectedMarkers(out, board, markerCorners, markerIds, rejectedCorners, cameraParams.matrix, cameraParams.distCoeffs, 10.0f, 3.0f, true, cv::noArray(), ARUCO_PARAMS);

    if(!markerIds.empty())
    {
        // Get the marker positions within the board
        std::vector<int> charucoIds;
        std::vector<cv::Point2f> charucoCorners;
        cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, out, board, charucoCorners, charucoIds);
        if (!charucoIds.empty())
        {
            cv::aruco::drawDetectedCornersCharuco(out, charucoCorners, charucoIds, cv::Scalar(0, 0, 255));

            cv::Vec3d rvec, tvec;
            bool valid = cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraParams.matrix, cameraParams.distCoeffs, rvec, tvec);

            float angle = rvec[1] * (180.0/CV_PI);

            std::cout << std::max(angle, lastAngle) - std::min(angle, lastAngle) << std::endl;
            if(std::max(angle, lastAngle) - std::min(angle, lastAngle) <= 100.0f || lastAngle == FLT_MAX)
            {
                lastAngle = angle;
            }
            else
            {
                angle = lastAngle;
            }

            for(int i = 0; i < 3; ++i)
            {
                std::cout << rvec[i] * (180.0/CV_PI) << " ";
            }
            std::cout << std::endl;

            if(valid)
            {
                cv::aruco::drawAxis(out, cameraParams.matrix, cameraParams.distCoeffs, rvec, tvec, 10.0f);
                out = rotateImg(out, angle);
            }
            else
                std::cout << "false" << std::endl;
        }
    }

    return out;
}