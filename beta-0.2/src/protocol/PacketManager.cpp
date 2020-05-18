//
// Created by apple on 2019/11/14.
//

#include "protocol/PacketManager.h"
#include <curses.h>

PacketManager::PacketManager(boost::asio::io_service& io_service)
    : udp_client_(io_service), registered_(), processed_packets() {}

bool PacketManager::Listen(std::uint16_t port) {
    if (udp_client_.Listen(port)) {
        Receive();
        return true;
    }
    return false;
}

void PacketManager::Send(const Packet &packet, bool async) {
    UdpBuffer buffer;
    packet >> buffer;
    udp_client_.Send(buffer.buffer(), buffer.size(), packet.endpoint_,
                    [this](const boost::system::error_code &error_code, size_t bytes) {
                        HandleReceive(error_code, bytes); }, async);
    if (packet.type_ == HandshakePacket::TYPE) {
        assert(((const HandshakePacket&)packet).status != 0);
    }
}

bool PacketManager::IsOpen() const {
    return udp_client_.IsOpen();
}

void PacketManager::Receive() {
    buffer_.clear();
    udp_client_.Receive(buffer_.buffer(), UdpBuffer::BUFFER_SIZE, endpoint_,
            [this](const boost::system::error_code &error_code, size_t bytes) {
        HandleReceive(error_code, bytes); });
}

void PacketManager::HandleSend(const boost::system::error_code &error_code, size_t /*bytes*/) {
    if (error_code) {
        // error message
    }
}

void PacketManager::HandleReceive(const boost::system::error_code &error_code, size_t bytes) {
    if (error_code) {
        // error message
    } else {
        buffer_.size(bytes);
        uint8_t type;
        buffer_ >> type;
        auto iter = registered_.find(type);
        if (iter != registered_.end()) {
            for (auto & callback : iter->second) {
                callback(buffer_, endpoint_);
            }
        } else {
            // error message
        }
    }
    Receive();
}

boost::asio::ip::udp::endpoint PacketManager::GetLocalEndpoint() const {
    return udp_client_.LocalEndpoint();
}