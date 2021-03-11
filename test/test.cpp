#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

void task(const std::string message)
{
    io_service io_service;
    tcp::socket socket(io_service);
    socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 8000 ));

    // request/message from client
    boost::system::error_code error;
    write( socket, buffer(message), error );
    if( !error )
    {
        std::cout << "client sent message!" << std::endl;
    }
    else
    {
        std::cerr << "send failed: " << error.message() << std::endl;
    }

    // getting response from server
    streambuf receive_buffer;
    read(socket, receive_buffer, transfer_all(), error);
    if( error && error != boost::asio::error::eof )
    {
        std::cerr << "receive failed: " << error.message() << std::endl;
    }
    else
    {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        std::cout << data << std::endl;
    }
}

int main()
{
    task("POST / HTTP/1.1\r\n"
         "Content-Type: application/json\r\n"
         "User-Agent: PostmanRuntime/7.26.10\r\n"
         "Accept: */*\r\n"
         "Postman-Token: 9151cea7-3c91-4422-9758-40a5d8b3db8c\r\n"
         "Host: 127.0.0.1:8000\r\n"
         "Accept-Encoding: gzip, deflate, br\r\n"
         "Connection: keep-alive\r\n"
         "Content-Length: 84\r\n"
         "\r\n"
         "{\n"
         "    \"action\": \"registration\",\n"
         "    \"login\": \"by_tema\",\n"
         "    \"password\": \"1234\"\n"
         "}");
    task("POST / HTTP/1.1\r\n"
         "Content-Type: application/json\r\n"
         "User-Agent: PostmanRuntime/7.26.10\r\n"
         "Accept: */*\r\n"
         "Postman-Token: 9151cea7-3c91-4422-9758-40a5d8b3db8c\r\n"
         "Host: 127.0.0.1:8000\r\n"
         "Accept-Encoding: gzip, deflate, br\r\n"
         "Connection: keep-alive\r\n"
         "Content-Length: 77\r\n"
         "\r\n"
         "{\n"
         "    \"action\": \"login\",\n"
         "    \"login\": \"by_tema\",\n"
         "    \"password\": \"1234\"\n"
         "}");
    task("GET / HTTP/1.1\r\n"
         "Content-Type: application/json\r\n"
         "User-Agent: PostmanRuntime/7.26.10\r\n"
         "Accept: */*\r\n"
         "Postman-Token: 9151cea7-3c91-4422-9758-40a5d8b3db8c\r\n"
         "Host: 127.0.0.1:8000\r\n"
         "Accept-Encoding: gzip, deflate, br\r\n"
         "Connection: keep-alive\r\n"
         "Content-Length: 84\r\n"
         "\r\n"
         "{\n"
         "    \"action\": \"registration\",\n"
         "    \"login\": \"by_tema\",\n"
         "    \"password\": \"1234\"\n"
         "}");
    task("POST / HTTP/1.1\r\n"
         "Content-Type: application/json\r\n"
         "User-Agent: PostmanRuntime/7.26.10\r\n"
         "Accept: */*\r\n"
         "Postman-Token: 9151cea7-3c91-4422-9758-40a5d8b3db8c\r\n"
         "Host: 127.0.0.1:8000\r\n"
         "Accept-Encoding: gzip, deflate, br\r\n"
         "Connection: keep-alive\r\n"
         "Content-Length: 78\r\n"
         "\r\n"
         "{\n"
         "    \"action\": \"logout\",\n"
         "    \"login\": \"by_tema\",\n"
         "    \"password\": \"1234\"\n"
         "}");
    task("GET / HTTP/1.1\r\n"
         "Content-Type: application/json\r\n"
         "User-Agent: PostmanRuntime/7.26.10\r\n"
         "Accept: */*\r\n"
         "Postman-Token: 9151cea7-3c91-4422-9758-40a5d8b3db8c\r\n"
         "Host: 127.0.0.1:8000\r\n"
         "Accept-Encoding: gzip, deflate, br\r\n"
         "Connection: keep-alive\r\n"
         "Content-Length: 84\r\n"
         "\r\n"
         "{\n"
         "    \"action\": \"registration\",\n"
         "    \"login\": \"by_tema\",\n"
         "    \"password\": \"1234\"\n"
         "}");
    return 0;
}
