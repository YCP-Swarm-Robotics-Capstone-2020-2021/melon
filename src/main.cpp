#include <thread>

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
            std::cerr << "arg1: <port>\n";
            return;
        }

        asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]), state);

        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
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