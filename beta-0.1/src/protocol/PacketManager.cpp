//
// Created by apple on 2019/11/14.
//

#include "protocol/PacketManager.h"


PacketManager::PacketManager(boost::asio::io_service& io_service)
    : udp_client_(io_service), registered_() {}

template <typename PacketType>
void PacketManager::RegisterPacket(std::function<void(const PacketType&)> handler) {
    auto iter = registered_.find(PacketType::TYPE);
    if (iter == registered_.end()) {
        iter = registered_.emplace(PacketType::TYPE, std::vector<PacketHandler>()).first;
    }
    iter->second.second.push_back(std::bind(
            &PacketManager::HandlePacket<PacketType>,
            std::placeholders::_1, std::placeholders::_2,
            handler));
}

bool PacketManager::Listen(std::uint16_t port) {
    if (udp_client_.Listen(port)) {
        Receive();
        return true;
    }
    return false;
}

void PacketManager::Send(const Packet &packet) {
    UdpBuffer buffer;
    packet >> buffer;
    udp_client_.Send(buffer.buffer(), buffer.size(), packet.endpoint_,
            std::bind(&PacketManager::HandleSend, this,
                    std::placeholders::_1, std::placeholders::_2));
}

bool PacketManager::IsOpen() const {
    return udp_client_.IsOpen();
}

void PacketManager::Receive() {
    buffer_.clear();
    udp_client_.Receive(buffer_.buffer(), UdpBuffer::BUFFER_SIZE, endpoint_,
            std::bind(&PacketManager::HandleReceive, this,
                    std::placeholders::_1, std::placeholders::_2));
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
        auto iter = registered_.find(bytes);
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