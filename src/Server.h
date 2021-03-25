#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;
using namespace boost::beast;
using namespace boost::placeholders;
using ip::tcp;

class Server
{
private:
    tcp::acceptor acceptor_;
    void start_accept()
    {
        connection_handler::pointer connection = connection_handler::create(acceptor_.get_io_service());

        acceptor_.async_accept(connection->socket(),
                               boost::bind(&Server::handle_accept, this, connection,
                                           boost::asio::placeholders::error));
    }
public:
    Server(boost::asio::io_context& io_context): acceptor_(io_context, tcp::endpoint(tcp::v4(), 8000))
    {
        start_accept();
    }
    void handle_accept(connection_handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            connection->start();
        }
        start_accept();
    }
};