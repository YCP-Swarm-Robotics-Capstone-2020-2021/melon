//
// Created by tim on 4/5/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_state_sys = StateVariables();
std::vector<std::string> input_state_sys;

TEST(StateSystemSuite, Saves_Deletes_Loads)
{
    //test saving state
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"set", "camera", "camera_matrix", "1,2,3,4,5,6,7,8,9"});
    command_handler::do_command(input_state_sys,testing_state_state_sys);

    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"set", "robot", "r1", "1,2,3,4"});
    command_handler::do_command(input_state_sys,testing_state_state_sys);

    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "5000"});
    command_handler::do_command(input_state_sys,testing_state_state_sys);

    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"save", "state", "tests_state"});
    std::string response = command_handler::do_command(input_state_sys,testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("current state saved"));

    //test deleting current state using 'current' keyword
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"delete", "state", "current"});
    response = command_handler::do_command(input_state_sys,testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("has been cleared"));
    ASSERT_EQ(testing_state_state_sys.robot.robots.size(), 0);
    ASSERT_EQ(testing_state_state_sys.collector.collectors.size(), 0);
    ASSERT_EQ(testing_state_state_sys.camera.camera_matrix.empty(), true);

    //test loading previously saved state
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"load", "state", "tests_state"});
    response = command_handler::do_command(input_state_sys,testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("current state loaded"));
    ASSERT_EQ(testing_state_state_sys.robot.robots.at("r1").size(), 4);
    ASSERT_EQ(testing_state_state_sys.camera.camera_matrix.at<double>(0), 1);
    ASSERT_EQ(testing_state_state_sys.camera.camera_matrix.at<double>(3), 4);
    ASSERT_EQ(testing_state_state_sys.collector.collectors.at("gcs").port(), 5000);
    ASSERT_EQ(testing_state_state_sys.collector.collectors.at("gcs").address().to_string(), "127.0.0.1");

    //test listing state
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"list", "state"});
    response = command_handler::do_command(input_state_sys, testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("Saved states:"));
    EXPECT_THAT(response, HasSubstr("tests_state"));
}

TEST(StateSystemSuite, Invalid_State_Given_To_Delete)
{
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"delete", "state", "doesnt_exist"});
    std::string response = command_handler::do_command(input_state_sys,testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("does not exist"));
}

TEST(StateSystemSuite, Invalid_Save_Using_Keyword) {
    input_state_sys.clear();
    input_state_sys.insert(input_state_sys.end(), {"save", "state", "current"});
    std::string response = command_handler::do_command(input_state_sys,testing_state_state_sys);
    EXPECT_THAT(response, HasSubstr("cannot be used"));
}
