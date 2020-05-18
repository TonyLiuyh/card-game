//
// Created by apple on 2019/11/14.
//

#ifndef PROTOCOL_TCPSERVER_H
#define PROTOCOL_TCPSERVER_H

#include "protocol/TcpConnection.h"
#include <unordered_map>

class TcpServer {
public:
    typedef std::function<void(TcpConnection*)> Handler;

    explicit TcpServer(boost::asio::io_service& io_service);

    TcpConnection::ptr CreateConnection(std::uint16_t port = 0);

    void RemoveConnection(std::uint16_t port);

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::uint16_t, TcpConnection::ptr> connections_;
};

#endif //PROTOCOL_TCPCLIENT_H
