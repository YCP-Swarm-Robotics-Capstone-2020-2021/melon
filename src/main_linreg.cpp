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
std::optional<std::pair<float, float>> linearRegression(const std::vector<cv::Point2f>& points);
float len(const cv::Point2f& a);
void normalize(cv::Point2f& a);
float dot(const cv::Point2f& a, const cv::Point2f& b);
float angleBetween(const cv::Point2f& a, const cv::Point2f& b);

const std::string CALIBRATION_IMAGES_DIR = "../calibrationImgs/";
const int CAMERA_ID = 1;
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
                aruco::drawDetectedCornersCharuco(frame, board->corners, board->ids, cv::Scalar(0, 0, 255));

                // Divide the detected board's points into columns
                std::vector<std::vector<cv::Point2f>> columns;
                for(int i = 0; i < board->ids.size(); ++i)
                {
                    if(columns.size() <= board->ids[i] % 4)
                        columns.resize((board->ids[i] % 4)+1, std::vector<cv::Point2f>());

                    columns[board->ids[i] % 4].push_back(board->corners[i]);
                }

                std::cout << "new" << std::endl;
                int i = 0;
                for(auto& column : columns)
                {
                    std::cout << "id: - " << i++ << " ";
                    for(auto& point : column)
                    {
                        std::cout << point << ", ";
                    }
                    std::cout << std::endl;
                }

                // Calculate a linear regression trend-line for each column
                std::vector<std::pair<cv::Point2f, cv::Point2f>> linRegLines;
                linRegLines.reserve(columns.size());
                bool wait = false;
                for(auto& column : columns)
                {
                    if(column.size() > 1)
                    {
                        auto linReg = linearRegression(column);
                        std::pair<cv::Point2f, cv::Point2f> line;
                        const cv::Point2f& p1 = column[0];
                        const cv::Point2f& p2 = column[column.size()-1];

                        // Calculate the beginning and end points of each trend-line
                        if(linReg.has_value())
                        {
                            auto [m, b] = linReg.value();
                            line.first.x = (p1.y - b) / m;
                            line.first.y = (m * p1.x) + b;

                            line.second.x = (p2.y - b) / m;
                            line.second.y = (m * p2.x) + b;

                            wait = (abs(line.first.x - p1.x) >= 20.0f) && (abs(line.second.x - p2.x) >= 20.0f);
                            if(wait)
                            {
                                std::cout << "new" << std::endl;
                                std::cout << "m: " << m << ", b: " << b << std::endl;
                                std::cout << line.first << std::endl;
                                std::cout << line.second << std::endl;
                                std::cout << "p1: " << p1 << std::endl;
                                std::cout << "p2: " << p2 << std::endl;
                                for(auto& point : column)
                                {
                                    std::cout << point << ", ";
                                }
                                std::cout << std::endl;
                            }
                        }
                        else
                        {
                            line.first = p1;
                            line.second = p2;
                        }


                        linRegLines.push_back(line);
                    }
                }

                // Draw each of the column lines
                for(auto& line : linRegLines)
                    cv::line(frame, line.first, line.second, cv::Scalar(0, 255, 0), 2);

                if(!linRegLines.empty())
                {
                    // Calculate and draw the average line from each of the columns
                    std::pair<cv::Point2f, cv::Point2f> avgLine;
                    for(auto& line : linRegLines)
                    {
                        avgLine.first.x += line.first.x;
                        avgLine.first.y += line.first.y;

                        avgLine.second.x += line.second.x;
                        avgLine.second.y += line.second.y;
                    }

                    avgLine.first.x /= linRegLines.size();
                    avgLine.first.y /= linRegLines.size();
                    avgLine.second.x /= linRegLines.size();
                    avgLine.second.y /= linRegLines.size();

                    cv::line(frame, avgLine.first, avgLine.second, cv::Scalar(255, 0, 0), 2);

                    // Calculate and draw a "normal" line to base rotations off of
                    std::pair<cv::Point2f, cv::Point2f> hNormal;
                    hNormal.first = cv::Point2f(frame.size().width / 2.0, frame.size().height);
                    hNormal.second = cv::Point2f(frame.size().width / 2.0, 0.0);
                    cv::line(frame, hNormal.first, hNormal.second, cv::Scalar(0, 0, 0));

                    // Calculate the normal and rotation vectors, then get the angle between them
                    cv::Point2f nVec(hNormal.second.x - hNormal.first.x, hNormal.second.y - hNormal.first.y);
                    cv::Point2f rVec(avgLine.second.x - avgLine.first.x, avgLine.second.y - avgLine.first.y);

                    float rotation = angleBetween(rVec, nVec) * (180.0/CV_PI);

                    std::stringstream ss;
                    ss << rotation << " degrees";
                    cv::putText(frame, ss.str(), cv::Point(1, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
                    if(wait)
                    {
                        cv::imshow("melon", frame);
                        cv::waitKey(0);
                    }
                }

/*                std::stringstream ss;

                aruco::estimatePoseCharucoBoard(board->corners, board->ids, calibrationBoard, calibration.matrix, calibration.distortion, board->rvec, board->tvec);
                aruco::drawAxis(frame, calibration.matrix, calibration.distortion, board->rvec, board->tvec, 10.0f);

                for(int i = 0; i < 3; ++i)
                    board->rvec[i] *= (180.0/CV_PI);
                ss = std::stringstream();
                ss << "R(x,y,z): " << board->rvec;
                cv::putText(frame, ss.str(), cv::Point(1, 45), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);

                ss = std::stringstream();

                ss << "T(x,y,z): " << board->tvec;
                cv::putText(frame, ss.str(), cv::Point(1, 70), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);

                ss = std::stringstream();
                ss << "{";
                ss << "\"id\":\"board0\", ";
                ss << "\"rvec\":\"" << board->rvec << "\", ";
                ss << "\"tvec\":\"" << board->tvec << "\"";
                ss << "}";*/
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
    std::cout << calibration.error << std::endl;

    return calibration;
}

std::optional<std::pair<float, float>> linearRegression(const std::vector<cv::Point2f>& points)
{
    float xsum = 0;
    float xEsum = 0;
    float ysum = 0;
    float prodsum = 0;
    for(auto point : points)
    {
        xsum += point.x;
        xEsum += point.x*point.x;
        ysum += point.y;
        prodsum += (point.x*point.y);
    }

    float m = ((points.size() * prodsum) - (xsum * ysum)) / ((points.size() * xEsum) - (xsum*xsum));
    float b = (ysum - (m * xsum)) / points.size();

    if(m == 0.0 || std::isinf(m) || std::isnan(m))
        return {};
    else
        return std::pair(m, b);
}

float len(const cv::Point2f& a)
{
    return (float)sqrt((a.x * a.x) + (a.y * a.y));
}
void normalize(cv::Point2f& a)
{
    float l = len(a);
    a.x /= l;
    a.y /= l;
}
float dot(const cv::Point2f& a, const cv::Point2f& b)
{
    return (a.x * b.x) + (a.y * b.y);
}
float angleBetween(const cv::Point2f& a, const cv::Point2f& b)
{
    cv::Point2f a1 = a;
    cv::Point2f b1 = b;
    normalize(a1);
    normalize(b1);
    return acos(dot(a1, b1));
}