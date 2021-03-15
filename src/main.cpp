#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <ctime>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "cmdhandler/logger.h"
#include "cmdhandler/server.h"
#include "camera/opencvcamera.h"

void update_state_variables(GlobalState& state);
void command_thread_func(int argc, char** argv, std::shared_ptr<GlobalState> state);
void camera_thread_func(std::shared_ptr<GlobalState> state);
int main(int argc, char** argv)
{
    //start logging
    //will open file stream and redirect stdout to said file.
    //logger::start();
    {
        // Assemble the log file's name based on the current date and time
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "./logs/log-";
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
        if (argc != 2){
            spdlog::critical("arg1: <port>");
            return;
        }

        asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]), state);

        io_context.run();
    }
    catch (std::exception& e){
        spdlog::critical("Exception: %s", e.what());
    }
}

void camera_thread_func(std::shared_ptr<GlobalState> state)
{
    StateVariables local_variables;
    state->apply(local_variables);

    // TODO: Connect to camera
    std::unique_ptr<Camera> camera = std::make_unique<OpenCvCamera>(local_variables);

    // TODO: Loop while camera should be connected (indicated within the state variables, there should be a variable
    //       that specifies if the user has requested a camera disconnect or not)
    bool loop = false;
    while (loop)
    {

    }
}