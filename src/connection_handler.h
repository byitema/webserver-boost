#ifndef WEBSERVER_BOOST_CONNECTION_HANDLER_H
#define WEBSERVER_BOOST_CONNECTION_HANDLER_H

#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/bind.hpp>

#include "../include/json.hpp"

#include "connection_handler.h"

using namespace boost::asio;
using namespace boost::beast;
using namespace boost::placeholders;
using ip::tcp;

using namespace nlohmann;

//(login, password)
std::map<std::string, std::string> users;
//(login, is_logged)
std::map<std::string, bool> users_logged;
std::string request_type(const std::string& message)
{
    const char *head = &message[0];
    const char *tail = &message[0];

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
    int length = -3;
    while(*tail != '}')
    {
        tail++;
        length++;
    }

    std::string body = message.substr(found, length);
    return body;
}

std::string registration(const json& json_body)
{
    auto it = users.find(json_body["login"].dump());
    if (it != users.end())
    {
        return "user with this login already exist";
    }

    users[json_body["login"].dump()] = json_body["password"].dump();
    users_logged[json_body["login"].dump()] = false;

    return "success";
}

std::string login(const json& json_body)
{
    auto it = users.find(json_body["login"].dump());
    if (it != users.end())
    {
        if (it->second == json_body["password"].dump())
        {
            if (users_logged[json_body["login"].dump()])
            {
                return "user already logged in";
            }
            else
            {
                users_logged[json_body["login"].dump()] = true;
                return "success";
            }
        }
        else
        {
            return "wrong password";
        }
    }

    return "no user with this login";
}

std::string logout(const json& json_body)
{
    auto it = users.find(json_body["login"].dump());
    if (it != users.end())
    {
        if (it->second == json_body["password"].dump())
        {
            if (!users_logged[json_body["login"].dump()])
            {
                return "user already logged out";
            }
            else
            {
                users_logged[json_body["login"].dump()] = false;
                return "success";
            }
        }
        else
        {
            return "wrong password";
        }
    }

    return "no user with this login";
}

std::string process_message(const std::string& message)
{
    std::string type = request_type(message);
    if (type == "GET")
    {
        bool is_anyone_logged = false;
        for (auto it = users_logged.begin(); it != users_logged.end(); ++it)
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
        /**
         * {
         *      "action": "a",
         *      "login": "l",
         *      "password": "p"
         * }
         **/

        std::string body = request_body(message);
        json json_body = json::parse(body);
        std::string response = "default";

        std::string action = json_body["action"].dump();
        std::size_t action_length = json_body["action"].dump().length();
        action = action.substr(1, action_length - 2);

        if (action == "registration")
        {
            response = registration(json_body);
        }
        else if (action == "login")
        {
            response = login(json_body);
        }
        else if (action == "logout")
        {
            response = logout(json_body);
        }

        return response;
    }

    return "default";
}

class connection_handler : public boost::enable_shared_from_this<connection_handler>
{
private:
    tcp::socket sock;
    std::string message="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\ndefault";
    enum { max_length = 4096 };
    char data[max_length];

public:
    typedef boost::shared_ptr<connection_handler> pointer;
    connection_handler(io_service& io_service):sock{io_service}{}

    static pointer create(io_service& io_service)
    {
        return pointer(new connection_handler(io_service));
    }

    tcp::socket& socket()
    {
        return sock;
    }

    void start()
    {
        sock.async_read_some(
                boost::asio::buffer(data, max_length),
                boost::bind(&connection_handler::handle_read,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
            std::cout << data << std::endl;
            std::string request(data);
            std::string response = process_message(request);

            std::stringstream response_body;
            response_body << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << response;

            this->message = response_body.str();
            std::cout << message << std::endl;

            sock.async_write_some(
                boost::asio::buffer(message, max_length),
                boost::bind(&connection_handler::handle_write,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

        } else {
            std::cerr << "error: " << err.message() << std::endl;
            sock.close();
        }
    }
    void handle_write(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err)
        {

        }
        else
        {
            std::cerr << "error: " << err.message() << std::endl;
            sock.close();
        }
    }
};

#endif //WEBSERVER_BOOST_CONNECTION_HANDLER_H
