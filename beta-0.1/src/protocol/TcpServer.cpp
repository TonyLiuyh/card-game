//
// Created by apple on 2019/11/16.
//

#include "protocol/TcpServer.h"

TcpServer::TcpServer(boost::asio::io_service &io_service)
    : io_service_(io_service), acceptor_(io_service_), connections_() {

}

TcpConnection::ptr TcpServer::CreateConnection(std::uint16_t port) {
    auto iter = connections_.find(port);
    if (iter == connections_.end()) {
        auto connection = TcpConnection::ptr(new TcpConnection(io_service_, acceptor_, port));
        connections_.emplace(connection->LocalEndpoint().port(), connection);
        return connection;
    } else {
        return iter->second;
    }
}

void TcpServer::RemoveConnection(std::uint16_t port) {
    auto iter = connections_.find(port);
    if (iter != connections_.end()) {
        iter->second->Close();
        connections_.erase(iter);
    }
}