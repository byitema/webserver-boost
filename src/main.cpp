#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "connection_handler.h"
#include "Server.h"

using namespace boost::asio;
using namespace boost::beast;
using namespace boost::placeholders;
using ip::tcp;

using namespace nlohmann;

int main()
{
    try
    {
        io_context io_context;
        Server server(io_context);
        io_context.run();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}