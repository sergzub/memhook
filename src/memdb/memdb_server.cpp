#include "memdb_server.hpp"
#include "memdb_session.hpp"
#include <boost/lexical_cast.hpp>

namespace memhook {

memdb_server::memdb_server(const boost::shared_ptr<mapped_storage> &storage,
            boost::asio::io_service& io_service,
            const char *host, int port)
        : io_service_(io_service)
        , acceptor_(io_service)
        , storage_(storage) {
    using namespace boost::asio::ip;
    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
    tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    start_accept();
}

void memdb_server::start_accept() {
    boost::shared_ptr<memdb_session> new_session(new memdb_session(storage_, io_service_));
    acceptor_.async_accept(new_session->get_socket(), bind(&memdb_server::handle_accept,
            this, new_session, boost::asio::placeholders::error));
}

void memdb_server::handle_accept(const boost::shared_ptr<memdb_session> &session,
        const boost::system::error_code& error) {
    if (error)
        std::cerr << error << std::endl;
    else
        session->start();

    start_accept();
}

} // memhook
