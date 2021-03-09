#include "cmdhandler/logger.h"
#include "cmdhandler/server.h"
#include <thread>


void server_thread(int argc, char** argv, std::shared_ptr<global_state> state)
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

void breakme_thread(std::shared_ptr<global_state> state)
{
    state_variables variables;
    while(true)
    {
        if(state->flag.load())
        {
            state->mutex.lock();

            variables = state->variables;

            for(auto elem : state->variables.robots)
            {
                std::cout << elem.first << " ";
                for(auto i : elem.second)
                {
                    std::cout << i << ", ";
                }
                std::cout << std::endl;
            }
            state->flag.store(false);
            state->mutex.unlock();
        }
        else
        {
            //std::cout << "no update" << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    //start logging
    //will open file stream and redirect stdout to said file.
    //logger::start();
    std::shared_ptr<global_state> state = std::make_shared<global_state>();

    std::thread server_t(server_thread, argc, argv, state);
    std::thread breakme(breakme_thread, state);

    server_t.join();
    return 0;
}