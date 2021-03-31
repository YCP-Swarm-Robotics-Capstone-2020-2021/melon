//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_robot_sys = StateVariables();
std::vector<std::string> input_robot_sys;

TEST(RobotSystemSuite, Sets_Robot)
{
    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r1", "1,2,3,4"});

    std::string response = command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    //check that response is correct and size of r1's value is correct
    EXPECT_THAT(response, HasSubstr("added with marker values"));
    ASSERT_EQ(testing_state_robot_sys.robot.robots.at("r1").size(), 4);

    //check values
    for(int i = 0; i < testing_state_robot_sys.robot.robots.at("r1").size(); i++){
        ASSERT_EQ(testing_state_robot_sys.robot.robots.at("r1").data()[i], i + 1);
    }

    testing_state_robot_sys = StateVariables();
}

TEST(RobotSystemSuite, Gets_Robot)
{
    input_robot_sys.clear();

    //insert initial robot
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r1", "1,2,3,4"});
    command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"get", "robot", "r1"});
    std::string response = command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    EXPECT_THAT(response, HasSubstr("r1:"));
    EXPECT_THAT(response, HasSubstr("1,2,3,4"));

    testing_state_robot_sys = StateVariables();
}

TEST(RobotSystemSuite, Deletes_Robot)
{
    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r1", "1,2,3,4"});

    command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"delete", "robot", "r1"});
    std::string response = command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    EXPECT_THAT(response, HasSubstr("has been removed"));
    ASSERT_EQ(testing_state_robot_sys.robot.robots.size(), 0);

    testing_state_robot_sys = StateVariables();
}

TEST(RobotSystemSuite, Lists_Robots)
{
    input_robot_sys.clear();

    //insert initial robot
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r1", "1,2,3,4"});
    command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r2", "10,2,1,5"});
    command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"list", "robot"});
    std::string response = command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    EXPECT_THAT(response, HasSubstr("r1:"));
    EXPECT_THAT(response, HasSubstr("1,2,3,4"));
    EXPECT_THAT(response, HasSubstr("r2:"));
    EXPECT_THAT(response, HasSubstr("10,2,1,5"));

    testing_state_robot_sys = StateVariables();
}

TEST(RobotSystemSuite, Invalid_Markers_Given)
{
    input_robot_sys.clear();
    input_robot_sys.insert(input_robot_sys.end(), {"set", "robot", "r1", "1,2,test,4"});

    std::string response = command_handler::do_command(input_robot_sys, testing_state_robot_sys);

    //check that response is correct and size of r1's value is correct
    EXPECT_THAT(response, HasSubstr("please provide"));
    EXPECT_THAT(response, HasSubstr("integers"));
}

