#include <iostream>
#include <WS2tcpip.h>
#include <boost/asio.hpp>
#include "../include/json.hpp"

using namespace boost::asio;
using namespace nlohmann;
using ip::tcp;

std::string read_(tcp::socket & socket)
{
    streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = buffer_cast<const char*>(buf.data());
    return data;
}

void send_(tcp::socket & socket, const std::string& message)
{
    const std::string msg = message + "\n";
    write(socket, buffer(message));
}

void parse_message(std::string& message, json& http_request)
{
    const char *head = &message[0];
    const char *tail = &message[0];

    // Find request type
    while (*tail != ' ') ++tail;
    http_request["Type"] = std::string(head, tail);

    // Find path
    while (*tail == ' ') ++tail;
    head = tail;
    while (*tail != ' ') ++tail;
    http_request["Path"] = std::string(head, tail);
}

void process_message(const std::string& message)
{
    std::string msg = message.substr(2, (message.length() - 3));

    const char *head = &msg[0];
    const char *tail = &msg[0];

    while (*tail != '=' && *tail != '\0') ++tail;
    std::string action = std::string(head, tail);

    if (action.compare("login") == 0)
    {
        //TODO: login();
    }
    else if (action.compare("register") == 0)
    {
        //TODO: register();
    }
    else if (action.compare("logout") == 0)
    {
        //TODO: logout();
    }
    else
    {
        //TODO
    }

    //std::cout << action << '\n';

}

int main()
{
    //init
    io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 8000 ));
    tcp::socket socket_(io_service);

    json http_request;
    while(true) {
        acceptor_.accept(socket_);

        std::string message = read_(socket_);
        std::cout << message << std::endl;
        parse_message(message, http_request);

        //stop server
        if (http_request["Path"].dump().compare("\"/exit\"") == 0)
        {
            socket_.close();
            break;
        }

        process_message(http_request["Path"].dump());

        send_(socket_, http_request["Path"].dump());

        socket_.close();
    }

    return 0;
}
