//
// Created by apple on 2019/11/11.
//

#ifndef PROTOCOL_PACKETMANAGER_H
#define PROTOCOL_PACKETMANAGER_H

#include <map>
#include <functional>
#include "protocol/Packets.h"
#include "protocol/UdpBuffer.h"
#include "protocol/UdpClient.h"

class PacketManager {
public:
    explicit PacketManager(boost::asio::io_service& io_service);

    template <typename PacketType>
    void RegisterPacket(std::function<void(const PacketType&)> handler);

    bool Listen(std::uint16_t port);

    void Send(const Packet& packet);

    bool IsOpen() const;

private:
    typedef std::function<void(UdpBuffer, const boost::asio::ip::udp::endpoint&)> PacketHandler;
    template <typename PacketType>
    static void HandlePacket(UdpBuffer buffer, const boost::asio::ip::udp::endpoint& remote,
            const std::function<void(const PacketType&)>& handler) {
        PacketType packet;
        packet << buffer;
        packet.endpoint_ = remote;
        handler(packet);
    }

    void Receive();

    void HandleSend(const boost::system::error_code& error_code, size_t bytes);

    void HandleReceive(const boost::system::error_code& error_code, size_t bytes);

private:
    UdpClient udp_client_;
    UdpBuffer buffer_;
    boost::asio::ip::udp::endpoint endpoint_;
    std::map<std::uint8_t, std::vector<PacketHandler>> registered_;
};

#endif //PROTOCOL_PACKETMANAGER_H
