//
// Created by apple on 2019/11/8.
//

#include <utility>
#include "protocol/Packets.h"

std::uint32_t Packet::new_transaction_id = 1;

std::uint32_t Packet::NewTransactionID() { return new_transaction_id++; }

Packet::Packet(std::uint8_t type)
    : type_(type)
    , transaction_id_(0)
    , is_request_(false)
    , endpoint_() {}

Packet::Packet(std::uint8_t type, boost::asio::ip::udp::endpoint endpoint)
    : type_(type)
    , transaction_id_(NewTransactionID())
    , is_request_(true)
    , endpoint_(std::move(endpoint)) {}

HandshakePacket::HandshakePacket()
    : Packet(TYPE)
    , host_name_() {}

HandshakePacket::HandshakePacket(const boost::asio::ip::udp::endpoint &endpoint)
    : Packet(TYPE, endpoint)
    , host_name_() {}

bool HandshakePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << is_request_ << host_name_;
        return true;
    } catch (...) {
        return false;
    }
}

bool HandshakePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> is_request_ >> host_name_;
        return true;
    } catch (...) {
        return false;
    }
}

SendFilePacket::SendFilePacket()
    : Packet(TYPE)
    , file_name_()
    , file_size_(0)
    , send_endpoint_()
    , accepted_(false)
    , receive_endpoint_() {}

SendFilePacket::SendFilePacket(const boost::asio::ip::udp::endpoint& endpoint,
        std::string file_name, std::uint64_t file_size,
        boost::asio::ip::tcp::endpoint  send_endpoint)
    : Packet(TYPE, endpoint)
    , file_name_(std::move(file_name))
    , file_size_(file_size)
    , send_endpoint_(std::move(send_endpoint))
    , accepted_(false)
    , receive_endpoint_() {}

bool SendFilePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << is_request_ << file_name_
            << file_size_ << send_endpoint_ << accepted_ << receive_endpoint_;
        return true;
    } catch (...) {
        return false;
    }
}

bool SendFilePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> is_request_ >> file_name_
            >> file_size_ >> send_endpoint_ >> accepted_ >> receive_endpoint_;
        return true;
    } catch (...) {
        return false;
    }
}