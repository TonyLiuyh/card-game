//
// Created by apple on 2019/11/11.
//

#include "protocol/UdpClient.h"

UdpClient::UdpClient(boost::asio::io_service &io_service)
    : socket_(io_service), io_service_(io_service) {
}

bool UdpClient::Listen(std::uint16_t port) {
    socket_.open(boost::asio::ip::udp::v4());
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address_v4(), port);
    boost::system::error_code error_code;

    socket_.bind(endpoint, error_code);
    socket_.set_option(boost::asio::socket_base::broadcast(true));
    if (error_code) {
        socket_.close();
        return false;
    }
    return true;
}

void UdpClient::Send(const char *buffer, size_t size,
                      const boost::asio::ip::udp::endpoint &endpoint,
                      const UdpClient::Handler &handler, bool async) {
    if (async) {
        socket_.async_send_to(boost::asio::buffer(buffer, size), endpoint, handler);
    } else {
        socket_.send_to(boost::asio::buffer(buffer, size), endpoint);
    }
}

void UdpClient::Receive(char *buffer, size_t size, boost::asio::ip::udp::endpoint &endpoint,
                         const UdpClient::Handler &handler) {
    socket_.async_receive_from(boost::asio::buffer(buffer, size), endpoint, handler);
}

bool UdpClient::IsOpen() const {
    return socket_.is_open();
}

bool UdpClient::Close() {
    boost::system::error_code error_code;
    socket_.close(error_code);
    return !error_code;
}

boost::asio::ip::udp::endpoint UdpClient::LocalEndpoint() const {
    return socket_.local_endpoint();
}