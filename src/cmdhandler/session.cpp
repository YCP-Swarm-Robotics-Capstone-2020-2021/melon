#include "session.h"
#include <spdlog/spdlog.h>
#include "command_handler.h"

session::session(tcp::socket socket, std::shared_ptr<GlobalState> state): socket_(std::move(socket)), m_state(state)
{

}

std::string session::get_command(const char data[], std::size_t length){
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

std::vector<std::string> session::tokenize_command_by_spaces(std::string command){
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
    spdlog::info(socket_.remote_endpoint().address().to_string()+" connected");
    // Write out an initial '>' character so that user input stands out
    // do_write() also runs do_read(), so the program will begin listening after this is sent
    do_write("> ", 2);
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
                                        // Log input from user
                                        spdlog::info(socket_.remote_endpoint().address().to_string()+": "+command);
                                        // Output stream
                                        std::stringstream output;

                                        if(command == "quit"){
                                            spdlog::info(socket_.remote_endpoint().address().to_string()+" disconnected");
                                            socket_.close();
                                        }else if(command == "clear"){
                                            std::string clear_string = "\033[2J\033[H";
                                            output << clear_string;
                                        }else {
                                            //if not 'quit', tokenize input by " "
                                            std::vector<std::string> tokens = tokenize_command_by_spaces(command);

                                            StateVariables local_variables = m_state->get_state();
                                            std::string response = command_handler::do_command(tokens, local_variables);
                                            m_state->receive(local_variables);

                                            output << response << "\n";
                                        }
                                        output << "> ";
                                        // Send the response
                                        do_write(output.str(), output.str().length());
                                    }else if(ec == asio::error::eof || ec == asio::error::connection_reset){
                                        spdlog::info(socket_.remote_endpoint().address().to_string()+" disconnected");
                                    }
                                });

}

/**
 *  Write out to a connection (session) instance
 *
 * @param msg message string to write out
 * @param length length of msg param
 */
void session::do_write(std::string msg, std::size_t length)
{
    auto self(shared_from_this());
    std::shared_ptr<std::string> pmsg = std::make_shared<std::string>(msg);
    asio::async_write(socket_, asio::buffer(pmsg->data(), length),
                      [this, self, pmsg](asio::error_code ec, std::size_t length)
                      {
                          if (!ec)
                          {
                              // Log what was sent to user
                              if(*pmsg != "> ")
                              {
                                  size_t pos;
                                  // If end of message is "\n> ", remove it before logging
                                  if((pos = pmsg->find("\n> ")) != std::string::npos){
                                      pmsg->erase(pos, 3);
                                  }
                                  // If end of message is "> ", remove it before logging
                                  else if((pos = pmsg->find("> ")) != std::string::npos){
                                      pmsg->erase(pos, 2);
                                  }

                                  if(!pmsg->empty())
                                      spdlog::info("{} >> \n{}", socket_.remote_endpoint().address().to_string(), *pmsg);
                              }

                              // Wait for more input
                              do_read();
                          }
                      });
}