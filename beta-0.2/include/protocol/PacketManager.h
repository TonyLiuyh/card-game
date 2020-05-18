//
// Created by apple on 2019/11/11.
//

#ifndef PROTOCOL_PACKETMANAGER_H
#define PROTOCOL_PACKETMANAGER_H

#include <map>
#include <unordered_set>
#include <queue>
#include <functional>
#include "protocol/Packets.h"
#include "protocol/UdpBuffer.h"
#include "protocol/UdpClient.h"

class PacketManager {
public:
    explicit PacketManager(boost::asio::io_service& io_service);

    template <typename PacketType>
    void RegisterPacket(std::function<void(const PacketType&)> handler) {
        auto iter = registered_.find(PacketType::GetType());
        if (iter == registered_.end()) {
            iter = registered_.emplace(PacketType::GetType(), std::vector<PacketHandler>()).first;
        }
        iter->second.push_back(
                [this, handler](const UdpBuffer& buffer, const boost::asio::ip::udp::endpoint& remote) {
            HandlePacket<PacketType>(buffer, remote, handler);
        });
    }

    bool Listen(std::uint16_t port);

    void Send(const Packet& packet, bool async = true);

    bool IsOpen() const;

    boost::asio::ip::udp::endpoint GetLocalEndpoint() const;

private:
    typedef std::function<void(UdpBuffer, const boost::asio::ip::udp::endpoint&)> PacketHandler;

    template <typename PacketType>
    void HandlePacket(UdpBuffer buffer, const boost::asio::ip::udp::endpoint& remote,
            const std::function<void(const PacketType&)>& handler) {
        PacketType packet;
        if (packet << buffer) {
            packet.endpoint_ = remote;
            auto record = processed_packets.find(remote);
            if (record == processed_packets.end()) {
                record = processed_packets.emplace(remote, std::pair<std::queue<uint32_t>,
                        std::unordered_set<uint32_t>>()).first;
            }
            std::unordered_set<uint32_t>& transaction_set = record->second.second;
            std::queue<uint32_t>& transaction_queue = record->second.first;
            if (transaction_set.find(packet.transaction_id_) == transaction_set.end()) {
                if (transaction_queue.size() >= 10) {
                    transaction_set.erase(transaction_queue.front());
                    transaction_queue.pop();
                }
                transaction_queue.push(packet.transaction_id_);
                transaction_set.emplace(packet.transaction_id_);
                handler(packet);
            }
        }
    }

    void Receive();

    void HandleSend(const boost::system::error_code& error_code, size_t bytes);

    void HandleReceive(const boost::system::error_code& error_code, size_t bytes);

private:
    UdpClient udp_client_;
    UdpBuffer buffer_;
    boost::asio::ip::udp::endpoint endpoint_;
    std::map<std::uint8_t, std::vector<PacketHandler>> registered_;
    std::map<boost::asio::ip::udp::endpoint, std::pair<std::queue<uint32_t>, std::unordered_set<uint32_t>>> processed_packets;
};

#endif //PROTOCOL_PACKETMANAGER_H
