//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"

using ::testing::HasSubstr;

class CollectorSystemSuite : public testing::Test{
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

StateVariables CollectorSystemSuite::testing_state;

/**
 * Check a collector gets set correctly and state values are correct
 */
TEST_F(CollectorSystemSuite, Sets_Collector)
{
    //insert initial collector
    std::string response = command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "5000"}, testing_state);

    //expect that response string includes correct ip/port/name, also that state has correct values
    EXPECT_THAT(response, HasSubstr("added with ip"));
    ASSERT_EQ(testing_state.collector.collectors.at("gcs").address().to_string(), "127.0.0.1");
    ASSERT_EQ(testing_state.collector.collectors.at("gcs").port(), 5000);
}

/**
 * Test that response indicates correct values when getting a collector
 */
TEST_F(CollectorSystemSuite, Gets_Collector)
{
    //insert initial collector
    command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "5000"}, testing_state);

    //do the get command on collector that was just made
    std::string response = command_handler::do_command({"get", "collector", "gcs"}, testing_state);

    //expect that response string includes given values
    EXPECT_THAT(response, HasSubstr("gcs:"));
    EXPECT_THAT(response, HasSubstr("127.0.0.1"));
    EXPECT_THAT(response, HasSubstr("5000"));
}

/**
 * Test that a collector can be deleted/removed from state
 */
TEST_F(CollectorSystemSuite, Deletes_Collector)
{
    //insert initial collector
    command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "5000"}, testing_state);

    //delete said collector we just added
    std::string response = command_handler::do_command({"delete", "collector", "gcs"}, testing_state);

    //expect that response string indicates removal, and that state 'collectors' map is empty
    EXPECT_THAT(response, HasSubstr("has been removed"));
    ASSERT_EQ(testing_state.collector.collectors.size(), 0);
}

/**
 * Test that response indicates correct values/names when listing collectors
 */
TEST_F(CollectorSystemSuite, Lists_Collectors)
{
    //add initial collectors
    command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "5000"}, testing_state);
    command_handler::do_command({"set", "collector", "gcs2", "10.1.1.1", "8080"}, testing_state);

    //list said collectors
    std::string response = command_handler::do_command({"list", "collector"}, testing_state);

    //expect that response string includes correct values/ports/names
    EXPECT_THAT(response, HasSubstr("gcs:"));
    EXPECT_THAT(response, HasSubstr("127.0.0.1"));
    EXPECT_THAT(response, HasSubstr("5000"));
    EXPECT_THAT(response, HasSubstr("gcs2:"));
    EXPECT_THAT(response, HasSubstr("10.1.1.1"));
    EXPECT_THAT(response, HasSubstr("8080"));
}

/**
 * Test that an invalid ip isn't accepted
 */
TEST_F(CollectorSystemSuite, Invalid_IP_Given)
{
    //insert initial collector with invalid ip
    std::string response = command_handler::do_command({"set", "collector", "gcs", "127.0.0.10.1", "5000"}, testing_state);

    //expect that response string indicates invalid ip address
    EXPECT_THAT(response, HasSubstr("valid ipv4 address"));
}

/**
 * Test than an invalid port isn't accepted
 */
TEST_F(CollectorSystemSuite, Invalid_Port_Given)
{
    //insert initial collector with invalid port
    std::string response = command_handler::do_command({"set", "collector", "gcs", "127.0.0.1", "70000"}, testing_state);

    //expect that response string indicates invalid port
    EXPECT_THAT(response, HasSubstr("valid port number"));
}

