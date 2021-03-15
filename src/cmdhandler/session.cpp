#include "session.h"
#include "logger.h"
#include "command_handler.h"

session::session(tcp::socket socket, std::shared_ptr<GlobalState> state): socket_(std::move(socket)), m_state(state)
{

}

std::string session::get_command(char data[], std::size_t length){
    std::string current_command;
    for (int i = 0; i < length; i++) {
        char c = data[i];
        if (c == '\n' || c == '\r') {
            //as soon as \n or \r is found stop building command
            return current_command;
        }else{
            current_command += c;
        }
    }
    return current_command;
}

std::vector<std::string> session::tokenize_command(std::string command){
    std::vector<std::string> tokens;

    std::string delimiter = " ";
    size_t last = 0;
    size_t next = 0;
    while ((next = command.find(delimiter, last)) !=
           std::string::npos) {
        tokens.push_back(command.substr(last, next - last));
        last = next + 1;
    }
    // get last token after loop, since last word won't have space after it
    tokens.push_back(command.substr(last));
    return tokens;
}

void session::start()
{
    logger::log_connection(socket_.remote_endpoint().address());
    do_write("> ", 2);
    do_read();
}

void session::do_read()
{
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
                                [this, self](asio::error_code ec, std::size_t length)
                                {
                                    if (!ec) {
                                        //get command from data stream
                                        std::string command = get_command(data_, length);

                                        if(command == "quit"){
                                            logger::log_disconnect(socket_.remote_endpoint().address());
                                            socket_.close();
                                        }else if(command == "clear"){
                                            logger::log_input(socket_.remote_endpoint().address(), command);
                                            std::string clear_string = "\033[2J\033[H";
                                            do_write(clear_string, clear_string.size());
                                        }else {
                                            logger::log_input(socket_.remote_endpoint().address(), command);

                                            //if not 'quit', tokenize input by " "
                                            std::vector<std::string> tokens = tokenize_command(command);

                                            StateVariables local_variables = m_state->get_state();
                                            //TODO: pass to some input/token handler class
                                            std::string response = command_handler::do_command(tokens, &local_variables);
                                            m_state->receive(local_variables);

                                            logger::log_output(socket_.remote_endpoint().address(), response);
                                            do_write(response+"\n", response.length()+2);
                                        }
                                        do_write("> ", 2);
                                    }else if(ec == asio::error::eof || ec == asio::error::connection_reset){
                                        logger::log_disconnect(socket_.remote_endpoint().address());
                                    }
                                });

}

void session::do_write(std::string msg, std::size_t length)
{
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(msg, length),
                      [this, self](asio::error_code ec, std::size_t length)
                      {
                          if (!ec)
                          {
                              do_read();
                          }
                      });
}