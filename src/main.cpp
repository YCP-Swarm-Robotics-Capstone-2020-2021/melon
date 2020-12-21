#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <tclap/CmdLine.h>
namespace aruco = cv::aruco;
#define GET_DICT(dict) cv::Ptr<aruco::Dictionary> dict = aruco::getPredefinedDictionary(aruco::dict)

struct DetectionResult
{
    cv::Ptr<aruco::Dictionary> dictionary;
    cv::Ptr<aruco::DetectorParameters> parameters;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners, rejected_corners;
};
struct Marker
{
    int id;
    std::vector<cv::Point2f> corners;
};
class Line
{
public:
    cv::Point2f p1;
    cv::Point2f p2;
    void draw(const cv::Mat& dest, const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 1) const
    {
        cv::line(dest, p1, p2, color, thickness);
    }
};
class Robot
{
public:
    // 0: Top left, 1: Bottom left, 2: Top right, 3: Bottom right
    std::vector<int> ids;
    cv::Point2f position;
    float rotation;
    // Generate the lines to be used for rotation
    // marker_map -> hashmap of the detected markers
    // out -> generated lines will be inserted here
    // rotation_offset -> what the final calculated rotation should be offset by
    void make_lines(std::unordered_map<int, std::vector<cv::Point2f>>& marker_map, std::vector<Line> &out, float& rotation_offset) const;
};

struct DetectionResult detect_markers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters);
float len(const cv::Point2f& p);
cv::Point2f normalize(cv::Point2f& p);
float angle_between(const cv::Point2f& p1, const cv::Point2f& p2);

const int WAIT_TIME = 1;
int main(int argc, char** argv)
{
    cv::Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
    GET_DICT(DICT_4X4_50);
    GET_DICT(DICT_6X6_250);

    std::vector<struct Robot> robots;
/*    robots.push_back(Robot
     {
        .ids = {0, 1, 5, 6}
     });
    robots.push_back(Robot
     {
             .ids = {10, 11, 15, 16}
     });*/
    robots.push_back(Robot
     {
             .ids = {0, 1, 2, 3}
     });

    cv::VideoCapture videoFeed;
    videoFeed.set(cv::CAP_PROP_BUFFERSIZE, 0);

    std::string rtsp_username, rtsp_password;
    try
    {
        TCLAP::CmdLine cmd("melon");
        TCLAP::ValueArg<std::string> usernameArg("u", "username", "Camera RTSP stream login usernameArg", true, "", "string");
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
        std::string stream_url = "rtsp://"+rtsp_username+":"+rtsp_password+"@192.168.1.108/cam/realmonitor?channel=1&subtype=0";
        std::string gstreamer_components = " latency=0 ! rtph264depay ! h264parse ! decodebin ! videoconvert ! appsink drop=true";
        videoFeed.open("rtspsrc location=" + stream_url + gstreamer_components, cv::CAP_GSTREAMER);
    }
#endif

    cv::namedWindow("melon");
    cv::Mat frame;

    while(videoFeed.grab())
    {
        videoFeed.retrieve(frame);
        Line horizontal_line;
        horizontal_line.p1 = cv::Point2f(frame.size().width, frame.size().height / 2.0);
        horizontal_line.p2 = cv::Point2f(0.0, frame.size().height / 2.0);
        horizontal_line.draw(frame, cv::Scalar(0, 0, 0));
        cv::Point2f horizontal_vec(horizontal_line.p2.x - horizontal_line.p1.x, horizontal_line.p2.y - horizontal_line.p1.y);

        Line vertical_line;
        vertical_line.p1 = cv::Point2f(frame.size().width / 2.0, 0.0);
        vertical_line.p2 = cv::Point2f(frame.size().width / 2.0, frame.size().height);
        vertical_line.draw(frame, cv::Scalar(0, 0, 0));
        cv::Point2f vertical_vec(vertical_line.p2.x - vertical_line.p1.x, vertical_line.p2.y - vertical_line.p1.y);

        //auto detection_result = detect_markers(frame, DICT_6X6_250, params);
        auto detection_result = detect_markers(frame, DICT_4X4_50, params);
        aruco::drawDetectedMarkers(frame, detection_result.corners, detection_result.ids);

        std::unordered_map<int, std::vector<cv::Point2f>> marker_map;
        for(int i = 0; i < detection_result.ids.size(); ++i)
        {
            marker_map[detection_result.ids[i]] = detection_result.corners[i];
        }

        if(!detection_result.ids.empty())
        {
            aruco::drawDetectedMarkers(frame, detection_result.corners, detection_result.ids);

            for(int i = 0; i < robots.size(); ++i)
            {
                auto& robot = robots[i];

                std::vector<Line> lines;
                float rotation_offset;
                robot.make_lines(marker_map, lines, rotation_offset);


                struct Line avg_line;
                for(auto& line : lines)
                {
                    line.draw(frame, cv::Scalar(0, 0, 255), 2);

                    avg_line.p1.x += line.p1.x;
                    avg_line.p1.y += line.p1.y;
                    avg_line.p2.x += line.p2.x;
                    avg_line.p2.y += line.p2.y;
                }
                avg_line.p1.x /= lines.size();
                avg_line.p1.y /= lines.size();
                avg_line.p2.x /= lines.size();
                avg_line.p2.y /= lines.size();

                avg_line.draw(frame, cv::Scalar(255, 255, 0), 2);

                cv::Point2f avg_vec(avg_line.p2.x - avg_line.p1.x, avg_line.p2.y - avg_line.p1.y);

                float horizontal_theta = angle_between(horizontal_vec, avg_vec);
                float vertical_theta = angle_between(vertical_vec, avg_vec);

                // Todo: Each quadrant shouldn't be inclusive on both ends
                //      of the range
                bool quadrant1 = vertical_theta >= 0.0 && vertical_theta <= 90.0 &&
                                    horizontal_theta >= 0.0 && horizontal_theta <= 90.0;
                bool quadrant2 = vertical_theta >= 0.0 && vertical_theta <= 90.0 &&
                                    horizontal_theta >= 90.0 && horizontal_theta <= 180.0;
                bool quadrant3 = vertical_theta >= 90.0 && vertical_theta <= 180.0 &&
                                    horizontal_theta >= 90.0 && horizontal_theta <= 180.0;
                bool quadrant4 = vertical_theta >= 90.0 && vertical_theta <= 180.0 &&
                                    horizontal_theta >= 0.0 && horizontal_theta <= 90.0;

                if(quadrant1 || quadrant2)
                {
                    robot.rotation = horizontal_theta;
                }
                else if(quadrant3)
                {
                    robot.rotation = vertical_theta + 90.0;
                }
                else if(quadrant4)
                {
                    robot.rotation = 360.0 - horizontal_theta;
                }
                robot.rotation += rotation_offset / lines.size();

                std::stringstream ss;
                ss << "Robot [";
                for(int id : robot.ids)
                    ss << id << ",";
                ss << "]" << robot.rotation << " degrees";
                cv::putText(frame, ss.str(), cv::Point(1, 15*(i+1)), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);

            }
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

float len(const cv::Point2f& p)
{
    return (float) sqrt((p.x * p.x) + (p.y * p.y));
}

cv::Point2f normalize(const cv::Point2f &p)
{
    float l = len(p);
    return cv::Point2f(p.x / l, p.y / l);
}

float angle_between(const cv::Point2f& p1, const cv::Point2f& p2)
{
    return std::acos(normalize(p1).dot(normalize(p2))) * (180.0/CV_PI);
}

void Robot::make_lines(std::unordered_map<int, std::vector<cv::Point2f>>& marker_map, std::vector<Line> &out, float& rotation_offset) const
{
    rotation_offset = 0.0;
    bool id_found[4] = {};
    for(int i = 0; i < 4; ++i)
        id_found[i] = marker_map.find(ids[i]) != marker_map.end();

    for(int i = 0; i < 4; ++i)
    {
        if(id_found[i])
        {
            // left & right edges
            out.push_back(Line{marker_map[ids[i]][0], marker_map[ids[i]][3]});
            out.push_back(Line{marker_map[ids[i]][1], marker_map[ids[i]][2]});

            // top & bottom edges
/*            out.push_back(Line{marker_map[ids[i]][0], marker_map[ids[i]][1]});
            out.push_back(Line{marker_map[ids[i]][3], marker_map[ids[i]][2]});
            rotation_offset -= 180.0;*/
        }
    }

    if(id_found[0] && id_found[1])
    {
        // connect left & right side gaps
        out.push_back(Line{marker_map[ids[0]][3], marker_map[ids[1]][0]});
        out.push_back(Line{marker_map[ids[0]][2], marker_map[ids[1]][1]});
        // connect left & right side opposing corners
        out.push_back(Line{marker_map[ids[0]][0], marker_map[ids[1]][3]});
        out.push_back(Line{marker_map[ids[0]][1], marker_map[ids[1]][2]});
    }

    if(id_found[2] && id_found[3])
    {
        // connect left & right side gaps
        out.push_back(Line{marker_map[ids[2]][3], marker_map[ids[3]][0]});
        out.push_back(Line{marker_map[ids[2]][2], marker_map[ids[3]][1]});
        // connect left & right side opposing corners
        out.push_back(Line{marker_map[ids[2]][0], marker_map[ids[3]][3]});
        out.push_back(Line{marker_map[ids[2]][1], marker_map[ids[3]][2]});
    }
}