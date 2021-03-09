#include <iostream>
#include <WS2tcpip.h>
#include <boost/asio.hpp>
#include "../include/json.hpp"

using namespace boost::asio;
using namespace nlohmann;
using ip::tcp;

std::string read_socket(tcp::socket & socket)
{
    streambuf buf;
    try
    {
        read_until(socket, buf, "\n");
    }
    catch (boost::wrapexcept<boost::system::system_error> ex)
    {
        std::cerr << ex.what() << '\n';
    }
    std::string data = buffer_cast<const char*>(buf.data());
    return data;
}

void write_socket(tcp::socket & socket, const std::string& message)
{
    const std::string msg = message + "\n";
    write(socket, buffer(message));
}

json parse_message(std::string& message)
{
    json http_request;
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

    return http_request;
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
    tcp::socket socket(io_service);

    json http_request;

    while(true) {
        acceptor_.accept(socket);

        std::string message = read_socket(socket);
        std::cout << message << std::endl;
        http_request = parse_message(message);

        process_message(http_request["Path"].dump());

        write_socket(socket, http_request["Path"].dump());

        socket.close();
    }
    return 0;
}
