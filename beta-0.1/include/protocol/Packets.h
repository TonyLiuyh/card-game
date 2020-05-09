//
// Created by apple on 2019/11/8.
//

#ifndef PROTOCOL_PACKETS_H
#define PROTOCOL_PACKETS_H

#include "protocol/UdpBuffer.h"
#include <boost/asio/ip/tcp.hpp>

class Packet {
public:
    static std::uint32_t NewTransactionID();

    explicit Packet(std::uint8_t type);

    Packet(std::uint8_t type, boost::asio::ip::udp::endpoint endpoint);

    virtual bool operator>>(UdpBuffer &buffer) const = 0;

    virtual bool operator<<(UdpBuffer &buffer) = 0;

public:
    std::uint8_t type_;
    std::uint32_t transaction_id_;
    bool is_request_;
    boost::asio::ip::udp::endpoint endpoint_;

private:
    static std::uint32_t new_transaction_id;
};

class HandshakePacket : public Packet {
public:
    const static std::uint8_t TYPE = 1;

    HandshakePacket();

    explicit HandshakePacket(const boost::asio::ip::udp::endpoint &endpoint);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    std::string host_name_; // response
};

class SendFilePacket : public Packet {
public:
    const static std::uint8_t TYPE = 2;

    SendFilePacket();

    SendFilePacket(const boost::asio::ip::udp::endpoint& endpoint, std::string file_name,
            std::uint64_t file_size, boost::asio::ip::tcp::endpoint send_endpoint);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    std::string file_name_;
    std::uint64_t file_size_;
    boost::asio::ip::tcp::endpoint send_endpoint_;
    // response
    bool accepted_;
    boost::asio::ip::tcp::endpoint receive_endpoint_;
};

#endif // PROTOCOL_PACKETS_H
