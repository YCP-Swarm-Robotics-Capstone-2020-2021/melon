#include "robotsystem.h"
#include "../constants/systems.h"
#include "../statevariables.h"

RobotSystem::RobotSystem() : System(ROBOT_SYS_CMD)
{


/*    register_cmd(ListCommand(
            [](const Command& cmd, StateVariables& variables)
            {
                std::string response = "Current robots:";
                for(const auto& robot : variables.robot.robots)
                {
                    response += "\n    " + robot.first + ": ";
                    for(const auto& marker_id: robot.second)
                    {
                        response += std::to_string(marker_id) + ", ";
                    }
                    response.pop_back();
                }
            }));*/
}