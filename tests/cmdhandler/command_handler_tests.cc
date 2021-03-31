//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_cmd_handler = StateVariables();
std::vector<std::string> input_cmd_handler;

TEST(CmdHandlerSuite, Invalid_System)
{
    input_cmd_handler.clear();
    input_cmd_handler.push_back("set");
    input_cmd_handler.push_back("wrongSystem");

    std::string response = command_handler::do_command(input_cmd_handler, testing_state_cmd_handler);

    EXPECT_THAT(response, HasSubstr("target system:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

TEST(CmdHandlerSuite, No_System_Given)
{
    input_cmd_handler.clear();
    input_cmd_handler.push_back("set");

    std::string response = command_handler::do_command(input_cmd_handler, testing_state_cmd_handler);

    ASSERT_EQ(response, "please provide a target system");
}

TEST(CmdHandlerSuite, Invalid_Command)
{
    input_cmd_handler.clear();
    input_cmd_handler.push_back("command");
    input_cmd_handler.push_back("robot");

    std::string response = command_handler::do_command(input_cmd_handler, testing_state_cmd_handler);

    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}

TEST(CmdHandlerSuite, Empty_Command)
{
    input_cmd_handler.clear();

    std::string response = command_handler::do_command(input_cmd_handler, testing_state_cmd_handler);

    EXPECT_THAT(response, HasSubstr("command:"));
    EXPECT_THAT(response, HasSubstr("not found"));
}