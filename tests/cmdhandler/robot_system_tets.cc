//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"
#include "../../src/cmdhandler/constants/commands.h"
#include "../../src/cmdhandler/constants/systems.h"

using ::testing::HasSubstr;

class RobotSystemSuite : public testing::Test{
protected:
    static void SetUpTestSuite() {
        testing_state = StateVariables();
    }

    void TearDown(){
        testing_state = StateVariables();
    }
public:
    static StateVariables testing_state;
};

StateVariables RobotSystemSuite::testing_state;

/**
 * Check that a robot gets set correctly with correct values
 */
TEST_F(RobotSystemSuite, Sets_Robot)
{
    //set a robot value to check
    std::string response = command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r1", "1,2,3,4"}, testing_state);

    //check that response is correct and size of r1's value is correct
    EXPECT_THAT(response, HasSubstr("added with marker values"));
    ASSERT_EQ(testing_state.robot.robots.at("r1").size(), 4);

    //check values
    for(int i = 0; i < testing_state.robot.robots.at("r1").size(); i++){
        ASSERT_EQ(testing_state.robot.robots.at("r1").data()[i], i + 1);
    }
}

/**
 * Test that response indicates correct values when getting a robot
 */
TEST_F(RobotSystemSuite, Gets_Robot)
{
    //insert initial robot
    command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r1", "1,2,3,4"}, testing_state);

    //get the robot back
    std::string response = command_handler::do_command({GET_CMD, ROBOT_SYS_CMD, "r1"}, testing_state);

    //expect the robot's name/values to be in response string
    EXPECT_THAT(response, HasSubstr("r1:"));
    EXPECT_THAT(response, HasSubstr("1,2,3,4"));
}

/**
 * Test that a robot can get removed from state
 */
TEST_F(RobotSystemSuite, Deletes_Robot)
{
    //set initial robot
    command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r1", "1,2,3,4"}, testing_state);

    //delete said robot
    std::string response = command_handler::do_command({DELETE_CMD, ROBOT_SYS_CMD, "r1"}, testing_state);

    //expect that response string indicates removal of robot, and the 'robots' map is empty
    EXPECT_THAT(response, HasSubstr("has been removed"));
    ASSERT_EQ(testing_state.robot.robots.size(), 0);
}

/**
 * Test that response indicates correct values when listing all robots
 */
TEST_F(RobotSystemSuite, Lists_Robots)
{
    //insert two robots
    command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r1", "1,2,3,4"}, testing_state);
    command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r2", "10,2,1,5"}, testing_state);

    //list out the said robots
    std::string response = command_handler::do_command({LIST_CMD, ROBOT_SYS_CMD}, testing_state);

    //expect the robots' names/values to be in response string
    EXPECT_THAT(response, HasSubstr("r1:"));
    EXPECT_THAT(response, HasSubstr("1,2,3,4"));
    EXPECT_THAT(response, HasSubstr("r2:"));
    EXPECT_THAT(response, HasSubstr("10,2,1,5"));
}

/**
 * Test that invalid marker values aren't accepted
 */
TEST_F(RobotSystemSuite, Invalid_Markers_Given)
{
    //give a non int value as a marker id, expect that response indicates invalid value given
    std::string response = command_handler::do_command({SET_CMD, ROBOT_SYS_CMD, "r1", "1,2,test,4"}, testing_state);

    EXPECT_THAT(response, HasSubstr("please provide"));
    EXPECT_THAT(response, HasSubstr("integers"));
}

