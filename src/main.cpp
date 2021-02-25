#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <tclap/CmdLine.h>
#include <opencv2/aruco/charuco.hpp>
namespace aruco = cv::aruco;
#define GET_DICT(dict) cv::Ptr<aruco::Dictionary> dict = aruco::getPredefinedDictionary(aruco::dict)

struct DetectionResult
{
    cv::Ptr<aruco::Dictionary> dictionary;
    cv::Ptr<aruco::DetectorParameters> parameters;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners, rejected_corners;
};

struct DetectionResult detect_markers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters);
const int WAIT_TIME = 1;
int main(int argc, char** argv)
{
    //cv::Mat camMatrix = (cv::Mat_<double>(3, 3) << 1926.563579128611, 0, 1380.786498984395, 0, 1895.646635370541, 745.4521161723168, 0, 0, 1);
    //cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << -0.5841261114485079, 0.5046795564194936, 0.01825235358115559, -0.009669823262374196, -0.2218050423802391);

/*    cv::Mat camMatrix = (cv::Mat_<double>(3, 3) << 2.9653741646688441e+03, 0., 1.4270650454469758e+03, 0.,
            2.9667567356365648e+03, 7.1224694118195373e+02, 0., 0., 1.);
    cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << -4.9156607356228843e-01, 6.3973165223843342e-01,
            2.9296922331222201e-03, -2.5833647279891840e-03,
            -4.0674636913688195e-01);*/

    cv::Mat camMatrix = (cv::Mat_<double>(3, 3) << 2.9785179800037645e+03, 0., 1.4119247677872459e+03, 0.,
            2.9751162563958610e+03, 7.0447947019337823e+02, 0., 0., 1.);
    cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << -4.2034395258899948e-01, 3.9075688853883211e-02,
            2.5246798597957452e-03, -2.6197970279735407e-03,
            3.3412521345259510e-01);

    camMatrix.at<double>(0, 0) *= (1920.0 / 2688.0);
    camMatrix.at<double>(1, 1) *= (1080.0 / 1520.0);

    cv::Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
    GET_DICT(DICT_4X4_50);
    GET_DICT(DICT_6X6_250);

    cv::VideoCapture videoFeed;
    videoFeed.set(cv::CAP_PROP_BUFFERSIZE, 0);

    std::string rtsp_username, rtsp_password;
    try
    {
        TCLAP::CmdLine cmd("melon");
        TCLAP::ValueArg<std::string> usernameArg("u", "username", "Camera RTSP stream login username", true, "", "string");
        TCLAP::ValueArg<std::string> passwordArg("p", "password", "Camera RTSP stream login password", true, "", "string");
        cmd.add(usernameArg);
        cmd.add(passwordArg);
        cmd.parse(argc, argv);

        rtsp_username = usernameArg.getValue();
        rtsp_password = passwordArg.getValue();
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "Error parsing RTSP arguments: " << e.error() << " argument: " << e.argId() << std::endl;
        return -1;
    }
//#define NO_GSTREAMER
#ifdef NO_GSTREAMER
    videoFeed.open("rtsp://"+rtsp_username+":"+rtsp_password+"@192.168.1.108/cam/realmonitor?channel=1&subtype=0");
#else
    // TODO: Look more into gstreamer pipeline components
    {
        std::string stream_url = "rtsp://"+rtsp_username+":"+rtsp_password+"@10.42.0.35/cam/realmonitor?channel=1&subtype=0";
        std::string gstreamer_components = " latency=0 ! decodebin ! videoconvert ! appsink drop=true";
        videoFeed.open("rtspsrc location=" + stream_url + gstreamer_components);
    }
#endif

    cv::namedWindow("melon");
    cv::Mat frame;

    cv::Rect roi;
    cv::Vec2d center;
    while(videoFeed.grab())
    {
        videoFeed.retrieve(frame);

        if(roi.x != 0)
            frame = frame(roi);

        auto detection_result = detect_markers(frame, DICT_4X4_50, params);
        aruco::drawDetectedMarkers(frame, detection_result.corners, detection_result.ids);

        std::unordered_map<int, std::vector<cv::Point2f>> marker_map;
        for(int i = 0; i < detection_result.ids.size(); ++i)
        {
            marker_map[detection_result.ids[i]] = detection_result.corners[i];
        }

        std::vector<cv::Vec3d> rvec, tvec;
        if(!detection_result.ids.empty())
        {
            aruco::estimatePoseSingleMarkers(detection_result.corners, 1.0, camMatrix, distCoeffs, rvec, tvec);
        }

        if(roi.x == 0 && marker_map.find(0) != marker_map.end() && marker_map.find(1) != marker_map.end())
        {
            roi.x = marker_map[0][2].x;
            roi.y = marker_map[0][2].y;
            roi.width = marker_map[1][0].x - roi.x;
            roi.height = marker_map[1][0].y - roi.y;

            cv::Vec3d topLeft;
            cv::Vec3d bottomRight;

            for(int i = 0; i < detection_result.ids.size(); ++i)
            {
                switch(detection_result.ids[i])
                {
                    case 0:
                        topLeft = tvec[i];
                        break;
                    case 1:
                        bottomRight = tvec[i];
                        break;
                }
            }
            center[0] = (topLeft[0] + bottomRight[0])/2.0;
            center[1] = (topLeft[1] + bottomRight[1])/2.0;
        }

        //cv::rectangle(frame, roi, cv::Scalar(0, 255, 0));
        std::cout << center << std::endl;
        cv::circle(frame, cv::Point2d(center), 5, cv::Scalar(0, 255, 0));

        for(int i = 0; i < detection_result.ids.size(); i++)
        {
            double w_unit = roi.width / 6.0;
            double y_unit = roi.height / 6.0;


            tvec[i][0] /= w_unit;
            tvec[i][1] /= y_unit;

            for(int j = 0; j < 3; ++j)
            {
                rvec[i][j] *= (180.0 / CV_PI);
            }
            std::stringstream ss;
            ss << "[id: ";
            ss << detection_result.ids[i];
            ss << "] T: ";
            ss << tvec[i];
            ss << " R: ";
            ss << rvec[i];
            cv::putText(frame, ss.str(), cv::Point(1, 50*(i+1)), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 1);
        }

        cv::resize(frame, frame, cv::Size(1280, 720));

        cv::imshow("melon", frame);

        if(cv::waitKey(WAIT_TIME) == 27)
            break;
    }

    return 0;
}

struct DetectionResult detect_markers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters)
{
    struct DetectionResult detected_markers =
            {
                    .dictionary = dictionary,
                    .parameters = parameters,
            };
    aruco::detectMarkers(
            image,
            dictionary,
            detected_markers.corners,
            detected_markers.ids,
            parameters,
            detected_markers.rejected_corners
    );
    return detected_markers;
}