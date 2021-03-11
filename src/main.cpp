#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "../include/json.hpp"
#include "../include/ctpl_stl.h"

using namespace boost::asio;
using namespace boost::beast;
using ip::tcp;

using namespace nlohmann;

//(login, password)
std::map<std::string, std::string> users;
//(login, is_logged)
std::map<std::string, bool> users_logged;

std::mutex process_mutex;
std::mutex write_mutex;
std::mutex read_mutex;

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

        process_mutex.lock();
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
        process_mutex.unlock();

        return response;
    }

    return "default";
}

void thread_job(int id, tcp::acceptor& acceptor_, tcp::socket& socket)
{
    std::string response;
    while (true) {
        acceptor_.accept(socket);
        std::string message = read_socket(socket);
        std::cout << id << std::endl << message << std::endl;

        response = process_message(message);

        std::stringstream response_body;
        response_body << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << response;

        write_socket(socket, response_body.str());
        socket.close();
    }
}

int main() {
    //init
    io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 8000));
    tcp::socket socket(io_service);

    int thread_count = 2;
    ctpl::thread_pool p(thread_count);

    for (int i = 0; i < thread_count; i++)
    {
        p.push(thread_job, std::ref(acceptor_), std::ref(socket));
    }

    return 0;
}