#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

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
struct Robot
{
    // 0: Top left, 1: Top right, 2: Bottom left, 3: Bottom right
    std::vector<int> ids;
    cv::Point2f position;
    float rotation;
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

struct DetectionResult detect_markers(const cv::Mat& image, cv::Ptr<aruco::Dictionary> dictionary, cv::Ptr<aruco::DetectorParameters> parameters);
float len(const cv::Point2f& p);
cv::Point2f normalize(cv::Point2f& p);
float angle_between(const cv::Point2f& p1, const cv::Point2f& p2);

const int CAMERA_ID = 1;
const int WAIT_TIME = 1;
int main()
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
             .ids = {0, 2, 1, 3}
     });

    cv::VideoCapture videoFeed;
    videoFeed.open(CAMERA_ID);

    while(videoFeed.grab())
    {
        cv::Mat frame;
        videoFeed.retrieve(frame);

        //auto detection_result = detect_markers(frame, DICT_6X6_250, params);
        struct DetectionResult detection_result = detect_markers(frame, DICT_4X4_50, params);
        aruco::drawDetectedMarkers(frame, detection_result.corners, detection_result.ids);

        std::unordered_map<int, std::vector<cv::Point2f>> marker_map;
        for(int i = 0; i < detection_result.ids.size(); ++i)
        {
            marker_map[detection_result.ids[i]] = detection_result.corners[i];
        }

        if(!detection_result.ids.empty())
        {
            aruco::drawDetectedMarkers(frame, detection_result.corners, detection_result.ids);

            Line horizontal_normal;
            horizontal_normal.p1 = cv::Point2f(frame.size().width / 2.0, frame.size().height);
            horizontal_normal.p2 = cv::Point2f(frame.size().width / 2.0, 0.0);
            horizontal_normal.draw(frame, cv::Scalar(0, 0, 0));

            for(int i = 0; i < robots.size(); ++i)
            {
                auto& robot = robots[i];
                // Make sure that all of the robot's ids have been found, other skip this robot
                // TODO: Not all ids need to be present for calculations
                bool all_found = true;
                for(int id : robot.ids)
                    if(marker_map.find(id) == marker_map.end())
                    {
                        all_found = false;
                        break;
                    }
                if(!all_found)
                    continue;

                std::vector<Line> lines;
                // First pair
                // first side
                lines.push_back(Line{marker_map[robot.ids[0]][0], marker_map[robot.ids[0]][1]});
                lines.push_back(Line{marker_map[robot.ids[1]][0], marker_map[robot.ids[1]][1]});
                // connect gap
                lines.push_back(Line{marker_map[robot.ids[0]][1], marker_map[robot.ids[1]][0]});
                // connect opposing points
                lines.push_back(Line{marker_map[robot.ids[0]][0], marker_map[robot.ids[1]][1]});

                // second side
                lines.push_back(Line{marker_map[robot.ids[0]][3], marker_map[robot.ids[0]][2]});
                lines.push_back(Line{marker_map[robot.ids[1]][3], marker_map[robot.ids[1]][2]});
                // connect gap
                lines.push_back(Line{marker_map[robot.ids[0]][2], marker_map[robot.ids[1]][3]});
                // connect opposing points
                lines.push_back(Line{marker_map[robot.ids[0]][3], marker_map[robot.ids[1]][2]});

                // Second pair
                // first side
                lines.push_back(Line{marker_map[robot.ids[2]][0], marker_map[robot.ids[2]][1]});
                lines.push_back(Line{marker_map[robot.ids[3]][0], marker_map[robot.ids[3]][1]});
                // connect gap
                lines.push_back(Line{marker_map[robot.ids[2]][1], marker_map[robot.ids[3]][0]});
                // connect opposing points
                lines.push_back(Line{marker_map[robot.ids[2]][0], marker_map[robot.ids[3]][1]});

                // second side
                lines.push_back(Line{marker_map[robot.ids[2]][3], marker_map[robot.ids[2]][2]});
                lines.push_back(Line{marker_map[robot.ids[3]][3], marker_map[robot.ids[3]][2]});
                // connect gap
                lines.push_back(Line{marker_map[robot.ids[2]][2], marker_map[robot.ids[3]][3]});
                // connect opposing points
                lines.push_back(Line{marker_map[robot.ids[2]][3], marker_map[robot.ids[3]][2]});

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

                cv::Point2f normal_vec(horizontal_normal.p2.x - horizontal_normal.p1.x, horizontal_normal.p2.y - horizontal_normal.p1.y);
                cv::Point2f avg_vec(avg_line.p2.x - avg_line.p1.x, avg_line.p2.y - avg_line.p1.y);
                float rotation = angle_between(normal_vec, avg_vec) * (180.0/CV_PI);

                std::stringstream ss;
                ss << "Robot [";
                for(int id : robot.ids)
                    ss << id << ",";
                ss << "]" << rotation << " degrees";
                cv::putText(frame, ss.str(), cv::Point(1, 15*(i+1)), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);

            }
        }

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
    return std::acos(normalize(p1).dot(normalize(p2)));
}