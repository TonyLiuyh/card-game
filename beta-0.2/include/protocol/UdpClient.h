//
// Created by apple on 2019/11/11.
//

#ifndef PROTOCOL_UDPCLIENT_H
#define PROTOCOL_UDPCLIENT_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

class UdpClient {
public:
    typedef boost::function<void(const boost::system::error_code&, size_t)> Handler;

    explicit UdpClient(boost::asio::io_service& io_service);

    bool Listen(std::uint16_t port = 0);

    void Send(const char* buffer, size_t size, const boost::asio::ip::udp::endpoint& endpoint,
            const Handler& handler, bool async = true);

    void Receive(char* buffer, size_t size,
            boost::asio::ip::udp::endpoint& endpoint, const Handler& handler);

    bool IsOpen() const;

    bool Close();

    boost::asio::ip::udp::endpoint LocalEndpoint() const;

private:
    boost::asio::ip::udp::socket socket_;

    boost::asio::io_service& io_service_;
};

#endif //PROTOCOL_UDPCLIENT_H
