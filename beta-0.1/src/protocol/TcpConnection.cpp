//
// Created by apple on 2019/11/9.
//

#include "protocol/TcpConnection.h"

TcpConnection::TcpConnection(boost::asio::io_service& io_service,
        boost::asio::ip::tcp::acceptor& acceptor, std::uint16_t port)
    : acceptor_(acceptor)
    , socket_(io_service,
          boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0)) {}

boost::asio::ip::tcp::socket& TcpConnection::Socket() {
    return socket_;
}

bool TcpConnection::Send(const char *buffer, size_t bytes) {
    boost::system::error_code error_code;
    socket_.write_some(boost::asio::buffer(buffer, bytes), error_code);
    return !error_code;
}

size_t TcpConnection::Receive(char *buffer, size_t bytes) {
    boost::system::error_code error_code;
    size_t received = socket_.read_some(boost::asio::buffer(buffer, bytes), error_code);
    if (error_code) {
        return received;
    } else {
        return 0;
    }
}

bool TcpConnection::Connect(const boost::asio::ip::tcp::endpoint &remote_endpoint) {
    boost::system::error_code error_code;
    socket_.connect(remote_endpoint, error_code);
    return !error_code;
}

bool TcpConnection::Accept() {
    boost::system::error_code error_code;
    acceptor_.accept(socket_, error_code);
    return !error_code;
}

void TcpConnection::AsyncSend(const char* buffer, size_t bytes, const Handler& handler) {
    socket_.async_write_some(boost::asio::buffer(buffer, bytes), handler);
}

void TcpConnection::AsyncReceive(char* buffer, size_t bytes, const Handler& handler) {
    socket_.async_read_some(boost::asio::buffer(buffer, bytes), handler);
}

void TcpConnection::AsyncConnect(const boost::asio::ip::tcp::endpoint& remote_endpoint,
             const ConnectHandler& handler) {
    socket_.async_connect(remote_endpoint, handler);
}

void TcpConnection::AsyncAccept(const AcceptHandler& handler) {
    acceptor_.async_accept(socket_, handler);
}

bool TcpConnection::Close() {
    boost::system::error_code error_code;
    socket_.close(error_code);
    return !error_code;
}

boost::asio::ip::tcp::endpoint TcpConnection::LocalEndpoint() const {
    return socket_.local_endpoint();
}

boost::asio::ip::tcp::endpoint TcpConnection::RemoteEndpoint() const {
    return socket_.remote_endpoint();
}