#include <iostream>
#include <map>
#include <WS2tcpip.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../include/json.hpp"

using namespace boost::asio;
using namespace boost::beast;
using namespace nlohmann;
using ip::tcp;

//(login, is_logged)
std::map<std::string, bool> users;

std::string read_socket(tcp::socket & socket)
{
    streambuf buf;
    try
    {
        read_until(socket, buf, "\n");
    }
    catch (boost::wrapexcept<boost::system::system_error>& ex)
    {
        std::cerr << ex.what() << '\n';
    }

    std::string data = buffers_to_string(buf.data());
    return data;
}

void write_socket(tcp::socket & socket, const std::string& message)
{
    const std::string msg = message + "\n";
    write(socket, buffer(message));
}

std::string request_type(const std::string& message)
{
    const char *head = &message[0];
    const char *tail = &message[0];

    // Find request type
    while (*tail != ' ') ++tail;

    return std::string(head, tail);
}

std::string request_body(const std::string& message)
{
    std::size_t found = message.find("Content-Length:");
    const char *tail = &message[found];
    while(*tail != '\r')
    {
        tail++;
        found++;
    }
    found += 4;
    std::string body = message.substr(found);
    return body;
}

std::string process_message(const std::string& message)
{
    std::string type = request_type(message);
    if (type == "GET")
    {
        bool is_anyone_logged = false;
        for (auto it = users.begin(); it != users.end(); ++it)
        {
            if (it->second)
            {
                is_anyone_logged = true;
                break;
            }
        }

        if (is_anyone_logged)
        {
            return "MY PASSWORD: 1234567890";
        }
    }
    else if (type == "POST")
    {
        std::string body = request_body(message);
        json json_body = json::parse(body);



        return json_body.dump();
    }

    return "default";
}

int main()
{
    //init
    io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 8000 ));
    tcp::socket socket(io_service);

    std::string response;
    while(true) {
        acceptor_.accept(socket);

        std::string message = read_socket(socket);
        std::cout << message << std::endl;

        response = process_message(message);

        std::stringstream response_body;
        response_body << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << response;
        write_socket(socket, response_body.str());

        socket.close();
    }

    return 0;
}
