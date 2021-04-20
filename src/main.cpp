// DO NOT REMOVE OR ADD ANY INCLUDES BEFORE THIS. Having asio at the top of the file in main fixes some potential
// segfaults that occur due to mismatching typedefs within asio when other headers are included before it
#include <asio.hpp>

#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <ctime>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "cmdhandler/server.h"
#include "camera/camerawrapper.h"
#include "collectorserver/collectorserver.h"
#include "cmdhandler/variables/variable.h"
#include "cmdhandler/systems/system.h"
#include "cmdhandler/commands/namedcmd.h"
const std::string LOG_DIR = "logs/";

/** @brief Callback function for the thread that the command handler runs in
 *
 * @param argc [in] argc parameter from main func
 * @param argv [in] argv parameter from main func
 * @param state [in] Shared pointer to the global state
 */
void command_thread_func(int argc, char** argv, std::shared_ptr<GlobalState> state);

/** @brief Callback function for the thread that the camera runs in
 *
 * @param state Shared pointer to the global state
 */
void camera_thread_func(std::shared_ptr<GlobalState> state);

int main(int argc, char** argv)
{
    //start logging
    //will open file stream and redirect stdout to said file.
    {
        if(!std::filesystem::exists(LOG_DIR)){
            std::error_code ec;
            if(!std::filesystem::create_directory(LOG_DIR, ec)){
                std::cerr << "Error creating log directory" << std::endl;
                std::cerr << ec.message() << std::endl;
                return -1;
            }
        }

        // Assemble the log file's name based on the current date and time
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "./"+LOG_DIR+"/log-";
        ss << std::put_time(std::localtime(&now_c), "%m-%d-%Y_%H-%M-%S");
        ss << ".txt";

        // Create a multi-threaded, combined logger that prints to both console and a file
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(ss.str()));
        auto logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_every(std::chrono::seconds(1));
    }
    std::shared_ptr<GlobalState> state = std::make_shared<GlobalState>();

    std::thread command_thread(command_thread_func, argc, argv, state);
    std::thread camera_thread(camera_thread_func, state);

    command_thread.join();
    camera_thread.join();

    return 0;
}

void command_thread_func(int argc, char** argv, std::shared_ptr<GlobalState> state)
{
    try{
        // Make sure that the user has specified a port for the server to run on
        if (argc != 2){
            spdlog::critical("arg1: <port>");
            return;
        }

        // Start the server
        asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]), state);
        io_context.run();
    }
    catch (std::exception& e){
        spdlog::critical("Exception in command handler thread: \n{}", e.what());
    }
}

void camera_thread_func(std::shared_ptr<GlobalState> state)
{
    // Wait for camera to be connected and necessary properties present
    state->wait([](const StateVariables& state)
                {
                    return state.camera.connected && !state.camera.type.empty() && !state.camera.source.empty();
                });

    StateVariables local_variables = state->get_state();

    try
    {
        CameraWrapper camera(local_variables);

        CollectorServer server(local_variables);

        bool loop = true;
        cv::Mat frame;
        // Main frame-grabber loop
        while (loop)
        {
            // Apply any changes to state variables
            if(state->apply(local_variables))
            {
                if(!local_variables.camera.connected)
                {
                    // Wait for camera to be connected and necessary properties present
                    state->wait([](const StateVariables& state)
                                {
                                    return state.camera.connected && !state.camera.type.empty() && !state.camera.source.empty();
                                });
                    // Update the local variables
                    state->apply(local_variables);
                }

                // Update the server and camera with the new state variables
                server.update_state(local_variables);
                camera.update_state(local_variables);
            }

            if(camera->get_frame(frame))
            {
                cv::resize(frame, frame, cv::Size(1280, 720));
                cv::imshow("camera", frame);
            }

            // TODO: Actually generate/get data
            std::string data = "data";
            server.send(data);

            if(cv::waitKey(1) == 27)
                loop = false;
        }
    }
    catch (std::exception& e)
    {
        spdlog::critical("Exception in camera thread: \n{}", e.what());
    }
}