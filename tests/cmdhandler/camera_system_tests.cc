//
// Created by tim on 3/31/21.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/cmdhandler/command_handler.h"
#include "../../src/cmdhandler/constants/commands.h"
#include "../../src/cmdhandler/constants/systems.h"
#include "../../src/cmdhandler/constants/variables.h"

using ::testing::HasSubstr;

class CameraSystemSuite : public testing::Test{
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

StateVariables CameraSystemSuite::testing_state;

/**
 * Check each variable gets set correctly into state and the values are correct
 */
TEST_F(CameraSystemSuite, Sets_Variables)
{
    std::string response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::SOURCE, "testingurl"}, testing_state);
    EXPECT_THAT(response, HasSubstr("source set to"));
    ASSERT_EQ(testing_state.camera.source, "testingurl");

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::MARKER_DICT, "6"}, testing_state);
    EXPECT_THAT(response, HasSubstr("'marker_dictionary' variable set"));
    ASSERT_EQ(testing_state.camera.marker_dictionary, 6);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::OPTIONS, "testing", "false"}, testing_state);
    EXPECT_THAT(response, HasSubstr("'testing' set to"));
    ASSERT_EQ(testing_state.camera.camera_options.at("testing"), false);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::CAM_MATRIX, "1,2,3,4,5,6,7,8,9"}, testing_state);
    EXPECT_THAT(response, HasSubstr("'camera_matrix' variable set"));
    ASSERT_DOUBLE_EQ(testing_state.camera.camera_matrix.at<double>(0), 1);
    ASSERT_DOUBLE_EQ(testing_state.camera.camera_matrix.at<double>(3), 4);
    ASSERT_NE(testing_state.camera.camera_matrix.at<double>(9), 100);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::DIST_MATRIX, "1,2,3,4,5"}, testing_state);
    EXPECT_THAT(response, HasSubstr("'distortion_matrix' variable set"));
    ASSERT_DOUBLE_EQ(testing_state.camera.distortion_matrix.at<double>(0), 1);
    ASSERT_DOUBLE_EQ(testing_state.camera.distortion_matrix.at<double>(3), 4);
    ASSERT_NE(testing_state.camera.distortion_matrix.at<double>(4), 100);
}

/**
 * Check response includes correct variables/values when listing camera system
 */
TEST_F(CameraSystemSuite, Lists_Variables)
{
    //set some variables to check for in listing response string
    command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::SOURCE, "testingurl"}, testing_state);
    command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::MARKER_DICT, "6"}, testing_state);
    command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::OPTIONS, "testing", "false"}, testing_state);
    command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::CAM_MATRIX, "1,2,3,4,5,6,7,8,9"}, testing_state);
    command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::DIST_MATRIX, "1,2,3,4,5"}, testing_state);

    //do command and check the response string
    std::string response = command_handler::do_command({LIST_CMD, CAMERA_SYS_CMD}, testing_state);
    EXPECT_THAT(response, HasSubstr("source: testingurl"));
    EXPECT_THAT(response, HasSubstr("marker_dictionary: 6"));
    EXPECT_THAT(response, HasSubstr("camera_matrix: 1,2,3,4,5,6,7,8,9"));
    EXPECT_THAT(response, HasSubstr("distortion_matrix: 1,2,3,4,5,"));
    EXPECT_THAT(response, HasSubstr("camera_options:"));
    EXPECT_THAT(response, HasSubstr("testing: false"));
}

/**
 * Check for:
 *  - too many values given
 *  - not enough values given
 *  - non double value given
 *
 *  For the camera_matrix and the distortion_matrix variables
 */
TEST_F(CameraSystemSuite, Invalid_Matrices_Given)
{
    //test if too many doubles given
    std::string response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::CAM_MATRIX, "1,2,3,4,5,6,7,8,9,10,11"}, testing_state);
    EXPECT_THAT(response, HasSubstr("9 doubles"));
    ASSERT_EQ(testing_state.camera.camera_matrix.empty(), true);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::DIST_MATRIX, "1,2,3,4,5,6,7,8,9"}, testing_state);
    EXPECT_THAT(response, HasSubstr("5 doubles"));
    ASSERT_EQ(testing_state.camera.distortion_matrix.empty(), true);

    //test if not enough doubles given
    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::CAM_MATRIX, "1,2,3"}, testing_state);
    EXPECT_THAT(response, HasSubstr("9 doubles"));
    ASSERT_EQ(testing_state.camera.camera_matrix.empty(), true);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::DIST_MATRIX, "1,2"}, testing_state);
    EXPECT_THAT(response, HasSubstr("5 doubles"));
    ASSERT_EQ(testing_state.camera.distortion_matrix.empty(), true);

    //test if non double value given
    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::CAM_MATRIX, "1,2,3,4,f,6,7,8,9"}, testing_state);
    EXPECT_THAT(response, HasSubstr("list of doubles"));
    ASSERT_EQ(testing_state.camera.camera_matrix.empty(), true);

    response = command_handler::do_command({SET_CMD, CAMERA_SYS_CMD, CameraSystemVars::DIST_MATRIX, "1,2,3,f,5"}, testing_state);
    EXPECT_THAT(response, HasSubstr("list of doubles"));
    ASSERT_EQ(testing_state.camera.distortion_matrix.empty(), true);
}
