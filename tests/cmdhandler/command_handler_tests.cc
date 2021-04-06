//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

class CmdHandlerSuite : public testing::Test{
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

StateVariables CmdHandlerSuite::testing_state;

/**
 * Test that response indicates invalid target system given
 */
TEST_F(CmdHandlerSuite, Invalid_System)
{
    //do a command with the wrong system
    std::string response = command_handler::do_command({"set", "wrongSystem"}, testing_state);

    //check that response string indicates error
    EXPECT_THAT(response, HasSubstr("target system:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

/**
 * Test that response indicates no target system given
 */
TEST_F(CmdHandlerSuite, No_System_Given)
{
    //do a command with no system given
    std::string response = command_handler::do_command({"set"}, testing_state);

    //check that response string indicates error
    ASSERT_EQ(response, "please provide a target system");
}

/**
 * Test that response indicates invalid command given
 */
TEST_F(CmdHandlerSuite, Invalid_Command)
{
    //do command with invalid command (first element in tokens vector)
    std::string response = command_handler::do_command({"command", "root"}, testing_state);

    //check that response string indicates invalid command given
    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

/**
 * Test that command_handler handles an empty command string correctly
 */
TEST_F(CmdHandlerSuite, Empty_Command)
{
    //do command with empty token vector
    std::string response = command_handler::do_command({""}, testing_state);

    //check that response handles correctly
    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}