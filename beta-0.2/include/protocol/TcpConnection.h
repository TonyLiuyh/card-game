//
// Created by apple on 2019/11/9.
//

#ifndef PROTOCOL_TCPCONNECTION_H
#define PROTOCOL_TCPCONNECTION_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <memory>

class TcpConnection {
public:
    typedef boost::function<void(const boost::system::error_code&, size_t bytes)> Handler;
    typedef boost::function<void(const boost::system::error_code&)> ConnectHandler;
    typedef boost::function<void(const boost::system::error_code&)> AcceptHandler;
    typedef std::shared_ptr<TcpConnection> ptr;

    friend class TcpServer;

    boost::asio::ip::tcp::socket& Socket();

    bool Connect(const boost::asio::ip::tcp::endpoint& remote_endpoint);

    bool Accept();

    //bool Accept(const boost::asio::ip::tcp::endpoint& endpoint);

    bool Send(const char* buffer, size_t bytes);

    size_t Receive(char* buffer, size_t bytes);

    void AsyncConnect(const boost::asio::ip::tcp::endpoint& remote_endpoint,
                      const ConnectHandler& handler);

    void AsyncAccept(const AcceptHandler& handler);

    //void AsyncAccept(const boost::asio::ip::tcp::endpoint& endpoint, const Handler& handler);

    void AsyncSend(const char* buffer, size_t bytes, const Handler& handler);

    void AsyncReceive(char* buffer, size_t bytes, const Handler& handler);

    bool Close();

    boost::asio::ip::tcp::endpoint LocalEndpoint() const;

    boost::asio::ip::tcp::endpoint RemoteEndpoint() const;

protected:
    boost::asio::ip::tcp::acceptor& acceptor_;
    boost::asio::ip::tcp::socket socket_;

private:
    TcpConnection(boost::asio::io_service& io_service,
                  boost::asio::ip::tcp::acceptor& acceptor, std::uint16_t port = 0);
};

#endif // PROTOCOL_TCPCONNECTION_H
