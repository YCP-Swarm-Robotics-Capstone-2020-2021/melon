//
// Created by tim on 4/5/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

class StateSystemSuite : public testing::Test{
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

StateVariables StateSystemSuite::testing_state;

/**
 * Test that current state gets saved, deleted, and loaded correctly
 */
TEST_F(StateSystemSuite, Saves_Deletes_Loads)
{
    //setup state to be saved, set a variable from each target system
    command_handler::do_command({"set", "camera", "camera_matrix", "1,2,3,4,5,6,7,8,9"},testing_state);
    command_handler::do_command({"set", "robot", "r1", "1,2,3,4"},testing_state);
    command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "5000"},testing_state);

    //send save command
    std::string response = command_handler::do_command({"save", "state", "tests_state"},testing_state);

    //expect that response string indicates state has been saved
    EXPECT_THAT(response, HasSubstr("current state saved"));

    //test deleting current state using 'current' keyword
    response = command_handler::do_command({"delete", "state", "current"},testing_state);

    //expect that response string indicates state has been cleared, and the current state reflects empty values
    EXPECT_THAT(response, HasSubstr("has been cleared"));
    ASSERT_EQ(testing_state.robot.robots.size(), 0);
    ASSERT_EQ(testing_state.collector.collectors.size(), 0);
    ASSERT_EQ(testing_state.camera.camera_matrix.empty(), true);

    //test loading previously saved state
    response = command_handler::do_command({"load", "state", "tests_state"},testing_state);

    //expect that response string indicates state has been loaded, and the current state reflects correct values
    //uses previously saved state 'tests_state'
    EXPECT_THAT(response, HasSubstr("current state loaded"));
    ASSERT_EQ(testing_state.robot.robots.at("r1").size(), 4);
    ASSERT_DOUBLE_EQ(testing_state.camera.camera_matrix.at<double>(0), 1);
    ASSERT_DOUBLE_EQ(testing_state.camera.camera_matrix.at<double>(3), 4);
    ASSERT_EQ(testing_state.collector.collectors.at("gcs").port(), 5000);
    ASSERT_EQ(testing_state.collector.collectors.at("gcs").address().to_string(), "127.0.0.1");

    //test listing state
    response = command_handler::do_command({"list", "state"}, testing_state);

    //expect that response indicates the 'tests_state' at least
    EXPECT_THAT(response, HasSubstr("Saved states:"));
    EXPECT_THAT(response, HasSubstr("tests_state"));
}

/**
 * Test that response string indicates a non-existent state was given to delete
 */
TEST_F(StateSystemSuite, Invalid_State_Given_To_Delete)
{
    std::string response = command_handler::do_command({"delete", "state", "doesnt_exist"},testing_state);
    EXPECT_THAT(response, HasSubstr("does not exist"));
}

/**
 * Test that saving a state with keyword 'current' fails, and response string indicates that
 */
TEST_F(StateSystemSuite, Invalid_Save_Using_Keyword) {
    std::string response = command_handler::do_command({"save", "state", "current"},testing_state);
    EXPECT_THAT(response, HasSubstr("cannot be used"));
}
