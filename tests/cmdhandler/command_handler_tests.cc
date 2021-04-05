//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_cmd_handler = StateVariables();

/**
 * Test that response indicates invalid target system given
 */
TEST(CmdHandlerSuite, Invalid_System)
{
    //do a command with the wrong system
    std::string response = command_handler::do_command({"set", "wrongSystem"}, testing_state_cmd_handler);

    //check that response string indicates error
    EXPECT_THAT(response, HasSubstr("target system:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

/**
 * Test that response indicates no target system given
 */
TEST(CmdHandlerSuite, No_System_Given)
{
    //do a command with no system given
    std::string response = command_handler::do_command({"set"}, testing_state_cmd_handler);

    //check that response string indicates error
    ASSERT_EQ(response, "please provide a target system");
}

/**
 * Test that response indicates invalid command given
 */
TEST(CmdHandlerSuite, Invalid_Command)
{
    //do command with invalid command (first element in tokens vector)
    std::string response = command_handler::do_command({"command", "root"}, testing_state_cmd_handler);

    //check that response string indicates invalid command given
    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

/**
 * Test that command_handler handles an empty command string correctly
 */
TEST(CmdHandlerSuite, Empty_Command)
{
    //do command with empty token vector
    std::string response = command_handler::do_command({""}, testing_state_cmd_handler);

    //check that response handles correctly
    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}