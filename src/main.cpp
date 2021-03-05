#include <iostream>
#include <WS2tcpip.h>
#include <boost/asio.hpp>
#include "../include/json.hpp"

using namespace boost::asio;
using namespace nlohmann;
using ip::tcp;

std::string read_(tcp::socket & socket) {
    streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = buffer_cast<const char*>(buf.data());
    return data;
}

void send_(tcp::socket & socket, const std::string& message) {
    const std::string msg = message + "\n";
    write(socket, buffer(message));
}

int main() {
    io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 8000 ));
    tcp::socket socket_(io_service);

    while(true) {
        acceptor_.accept(socket_);

        std::string message = read_(socket_);
        std::cout << message << std::endl;

        json j;
        j["Response"] = "This is response";
        j["JSON"] = "In JSON";
        j["Yep"] = "YO";

        send_(socket_, j.dump());

        socket_.close();
    }

    return 0;
}
