//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_camera_sys = StateVariables();
std::vector<std::string> input_camera_sys;

TEST(CameraSystemSuite, Sets_Variables)
{
    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "source", "testingurl"});
    std::string response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("source set to"));
    ASSERT_EQ(testing_state_camera_sys.camera.source, "testingurl");

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "marker_dictionary", "6"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("'marker_dictionary' variable set"));
    ASSERT_EQ(testing_state_camera_sys.camera.marker_dictionary, 6);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_options", "testing", "false"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("'testing' set to"));
    ASSERT_EQ(testing_state_camera_sys.camera.camera_options.at("testing"), false);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_matrix", "1,2,3,4,5,6,7,8,9"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("'camera_matrix' variable set"));
    ASSERT_EQ(testing_state_camera_sys.camera.camera_matrix.at<double>(0), 1);
    ASSERT_EQ(testing_state_camera_sys.camera.camera_matrix.at<double>(3), 4);
    ASSERT_NE(testing_state_camera_sys.camera.camera_matrix.at<double>(9), 100);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "distortion_matrix", "1,2,3,4,5"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("'distortion_matrix' variable set"));
    ASSERT_EQ(testing_state_camera_sys.camera.distortion_matrix.at<double>(0), 1);
    ASSERT_EQ(testing_state_camera_sys.camera.distortion_matrix.at<double>(3), 4);
    ASSERT_NE(testing_state_camera_sys.camera.distortion_matrix.at<double>(4), 100);

    testing_state_camera_sys = StateVariables();
}

TEST(CameraSystemSuite, Lists_Variables)
{
    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "source", "testingurl"});
    command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "marker_dictionary", "6"});
    command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_options", "testing", "false"});
    command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_matrix", "1,2,3,4,5,6,7,8,9"});
    command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "distortion_matrix", "1,2,3,4,5"});
    command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"list", "camera"});
    std::string response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("source: testingurl"));
    EXPECT_THAT(response, HasSubstr("marker_dictionary: 6"));
    EXPECT_THAT(response, HasSubstr("camera_matrix: 1,2,3,4,5,6,7,8,9"));
    EXPECT_THAT(response, HasSubstr("distortion_matrix: 1,2,3,4,5,"));
    EXPECT_THAT(response, HasSubstr("camera_options:"));
    EXPECT_THAT(response, HasSubstr("testing: false"));

    testing_state_camera_sys = StateVariables();
}

TEST(CameraSystemSuite, Invalid_Matrices_Given)
{
    //test if too many doubles given
    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_matrix", "1,2,3,4,5,6,7,8,9,10,11"});
    std::string response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("9 doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.camera_matrix.empty(), true);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "distortion_matrix", "1,2,3,4,5,6,7,8,9"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("5 doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.distortion_matrix.empty(), true);

    //test if not enough doubles given
    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_matrix", "1,2,3"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("9 doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.camera_matrix.empty(), true);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "distortion_matrix", "1,2"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("5 doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.distortion_matrix.empty(), true);

    //test if non double value given
    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "camera_matrix", "1,2,3,4,f,6,7,8,9"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("list of doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.camera_matrix.empty(), true);

    input_camera_sys.clear();
    input_camera_sys.insert(input_camera_sys.end(), {"set", "camera", "distortion_matrix", "1,2,3,f,5"});
    response = command_handler::do_command(input_camera_sys,testing_state_camera_sys);

    EXPECT_THAT(response, HasSubstr("list of doubles"));
    ASSERT_EQ(testing_state_camera_sys.camera.distortion_matrix.empty(), true);
}
