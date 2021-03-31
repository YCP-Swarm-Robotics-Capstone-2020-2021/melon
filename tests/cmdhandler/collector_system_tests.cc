//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

StateVariables testing_state_collector_sys = StateVariables();
std::vector<std::string> input_collector_sys;

TEST(CollectorSystemSuite, Sets_Collector)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "5000"});

    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("added with ip"));
    ASSERT_EQ(testing_state_collector_sys.collector.collectors.at("gcs").address().to_string(), "127.0.0.1");
    ASSERT_EQ(testing_state_collector_sys.collector.collectors.at("gcs").port(), 5000);

    testing_state_collector_sys = StateVariables();
}

TEST(CollectorSystemSuite, Gets_Collector)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "5000"});
    command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"get", "collector", "gcs"});
    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("gcs:"));
    EXPECT_THAT(response, HasSubstr("127.0.0.1"));
    EXPECT_THAT(response, HasSubstr("5000"));

    testing_state_collector_sys = StateVariables();
}

TEST(CollectorSystemSuite, Deletes_Collector)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "5000"});
    command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"delete", "collector", "gcs"});
    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("has been removed"));
    ASSERT_EQ(testing_state_collector_sys.collector.collectors.size(), 0);
}

TEST(CollectorSystemSuite, Lists_Collectors)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "5000"});
    command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs2", "10.1.1.1", "8080"});
    command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"list", "collector"});
    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("gcs:"));
    EXPECT_THAT(response, HasSubstr("127.0.0.1"));
    EXPECT_THAT(response, HasSubstr("5000"));
    EXPECT_THAT(response, HasSubstr("gcs2:"));
    EXPECT_THAT(response, HasSubstr("10.1.1.1"));
    EXPECT_THAT(response, HasSubstr("8080"));

    testing_state_collector_sys = StateVariables();
}

TEST(CollectorSystemSuite, Invalid_IP_Given)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.1", "5000"});
    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("valid ipv4 address"));

    testing_state_collector_sys = StateVariables();
}

TEST(CollectorSystemSuite, Invalid_Port_Given)
{
    input_collector_sys.clear();
    input_collector_sys.insert(input_collector_sys.end(), {"set", "collector", "gcs", "127.0.0.1", "70000"});
    std::string response = command_handler::do_command(input_collector_sys, testing_state_collector_sys);

    EXPECT_THAT(response, HasSubstr("valid port number"));

    testing_state_collector_sys = StateVariables();
}

