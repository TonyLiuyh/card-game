//
// Created by apple on 2019/11/8.
//

#ifndef PROTOCOL_PACKETS_H
#define PROTOCOL_PACKETS_H

#include "protocol/UdpBuffer.h"
#include "app/Player.h"
#include "app/CardDeck.h"

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

    static std::uint8_t GetType() { return TYPE; }

    HandshakePacket();

    HandshakePacket(boost::asio::ip::udp::endpoint endpoint, const Player &player);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    // player information
    std::string name;
    std::string host_name;
    uint8_t status;
};

class OfflinePacket : public Packet {
public:
    const static std::uint8_t TYPE = 2;

    static std::uint8_t GetType() { return TYPE; }

    OfflinePacket();

    explicit OfflinePacket(boost::asio::ip::udp::endpoint endpoint);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;
};

class RoomInfoPacket : public Packet {
public:
    const static std::uint8_t TYPE = 3;

    static std::uint8_t GetType() { return TYPE; }

    RoomInfoPacket();

    RoomInfoPacket(boost::asio::ip::udp::endpoint endpoint, RoomInfo room);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    RoomInfo room;
};

class RoomPacket : public Packet {
public:
    const static std::uint8_t TYPE = 4;

    static std::uint8_t GetType() { return TYPE; }

    RoomPacket();

    // request
    RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, std::string message = "");

    // response - success
    RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, Room room,
               boost::asio::ip::udp::endpoint action_endpoint, std::string message = "");

    // response - failure
    // param message: error message
    RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, Room room, std::string message);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    Room::Action action;

    // the following properties only use for response
    Room room;

    bool success;

    std::string message;

    boost::asio::ip::udp::endpoint action_endpoint;
};

class AcknowledgePacket : public Packet {
public:
    const static uint8_t TYPE = 5;

    static std::uint8_t GetType() { return TYPE; }

    AcknowledgePacket();

    AcknowledgePacket(const boost::asio::ip::udp::endpoint& endpoint, uint32_t transaction);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    uint32_t transaction;
};

class StartGamePacket : public Packet {
public:
    const static uint8_t TYPE = 6;

    static std::uint8_t GetType() { return TYPE; }

    StartGamePacket();

    StartGamePacket(const boost::asio::ip::udp::endpoint& endpoint,
            std::vector<std::vector<Card>> cards, uint8_t first_player);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    std::vector<std::vector<Card>> cards;
    uint8_t first_player;
};

class PlayCardsPacket : public Packet {
public:
    const static uint8_t TYPE = 7;

    static std::uint8_t GetType() { return TYPE; }

    PlayCardsPacket();

    PlayCardsPacket(const boost::asio::ip::udp::endpoint& endpoint,
            std::multiset<Card>  cards, uint8_t player_index);

    bool operator>>(UdpBuffer &buffer) const override;

    bool operator<<(UdpBuffer &buffer) override;

public:
    std::multiset<Card> cards;

    uint8_t player_index;
};

#endif // PROTOCOL_PACKETS_H
