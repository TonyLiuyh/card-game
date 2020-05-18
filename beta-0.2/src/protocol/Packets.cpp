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
    , is_request_(false)
    , endpoint_(std::move(endpoint)) {}

HandshakePacket::HandshakePacket()
    : Packet(TYPE)
    , host_name()
    , name()
    , status(0) {}


HandshakePacket::HandshakePacket(boost::asio::ip::udp::endpoint endpoint, const Player &player)
    : Packet(TYPE, std::move(endpoint))
    , name(player.name)
    , host_name(player.host_name)
    , status(player.status) {
    is_request_ = false;
}

bool HandshakePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << status << is_request_ << name << host_name;
        return true;
    } catch (...) {
        return false;
    }
}

bool HandshakePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> status >> is_request_ >> name >> host_name;
        return true;
    } catch (...) {
        return false;
    }
}

OfflinePacket::OfflinePacket() : Packet(TYPE) {}

OfflinePacket::OfflinePacket(boost::asio::ip::udp::endpoint endpoint)
    : Packet(TYPE, std::move(endpoint)) {}

bool OfflinePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_;
        return true;
    } catch (...) {
        return false;
    }
}

bool OfflinePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_;
        return true;
    } catch (...) {
        return false;
    }
}

RoomInfoPacket::RoomInfoPacket() : Packet(TYPE), room() {}

RoomInfoPacket::RoomInfoPacket(boost::asio::ip::udp::endpoint endpoint, RoomInfo room)
    : Packet(TYPE, std::move(endpoint)), room(std::move(room)) {}

bool RoomInfoPacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << room;
        return true;
    } catch (...) {
        return false;
    }
}

bool RoomInfoPacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> room;
        return true;
    } catch (...) {
        return false;
    }
}

RoomPacket::RoomPacket() : Packet(TYPE), action(Room::UPDATE), room()
    , success(false), message(), action_endpoint() {}

RoomPacket::RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, std::string message)
    : Packet(TYPE, std::move(endpoint))
    , action(action)
    , room()
    , success(false)
    , message(std::move(message))
    , action_endpoint() {
    is_request_ = true;
}

RoomPacket::RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, Room room,
        boost::asio::ip::udp::endpoint action_endpoint, std::string message)
    : Packet(TYPE, std::move(endpoint))
    , action(action)
    , room(std::move(room))
    , success(true)
    , message(std::move(message))
    , action_endpoint(std::move(action_endpoint)) {
    is_request_ = false;
    for (int i = 0; i < this->room.players.size(); ++i) {
        if (this->room.players[i] == endpoint_) {
            this->room.this_player_index = i;
            break;
        }
    }
}

RoomPacket::RoomPacket(boost::asio::ip::udp::endpoint endpoint, Room::Action action, Room room, std::string message)
    : Packet(TYPE, std::move(endpoint))
    , action(action)
    , room(std::move(room))
    , success(false)
    , message(std::move(message))
    , action_endpoint() {
    is_request_ = false;
}

bool RoomPacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << is_request_ << (uint8_t)action
            << room << success << message << action_endpoint;
        return true;
    } catch (...) {
        return false;
    }
}

bool RoomPacket::operator<<(UdpBuffer &buffer) {
    try {
        uint8_t temp;
        buffer >> transaction_id_ >> is_request_ >> temp >> room >> success >> message >> action_endpoint;
        action = static_cast<Room::Action>(temp);
        return true;
    } catch (...) {
        return false;
    }
}

AcknowledgePacket::AcknowledgePacket() : Packet(TYPE), transaction(0) {}

AcknowledgePacket::AcknowledgePacket(const boost::asio::ip::udp::endpoint & endpoint, uint32_t transaction)
    : Packet(TYPE, endpoint), transaction(transaction) {
}

bool AcknowledgePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << transaction;
        return true;
    } catch (...) {
        return false;
    }
}

bool AcknowledgePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer << transaction_id_ >> transaction;
        return true;
    } catch (...) {
        return false;
    }
}

StartGamePacket::StartGamePacket() : Packet(TYPE), cards(), first_player(0) {

}

StartGamePacket::StartGamePacket(const boost::asio::ip::udp::endpoint &endpoint,
        std::vector<std::vector<Card> > cards, uint8_t first_player)
    : Packet(TYPE, endpoint), cards(std::move(cards)), first_player(first_player) {

}

bool StartGamePacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << first_player << cards;
        return true;
    } catch (...) {
        return false;
    }
}

bool StartGamePacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> first_player >> cards;
        return true;
    } catch (...) {
        return false;
    }
}

PlayCardsPacket::PlayCardsPacket() : Packet(TYPE), cards(), player_index(0) {

}

PlayCardsPacket::PlayCardsPacket(const boost::asio::ip::udp::endpoint &endpoint,
        std::multiset<Card> cards, uint8_t player_index)
    : Packet(TYPE, endpoint)
    , cards(std::move(cards))
    , player_index(player_index) {
}

bool PlayCardsPacket::operator>>(UdpBuffer &buffer) const {
    try {
        buffer << type_ << transaction_id_ << player_index << cards;
        return true;
    } catch (...) {
        return false;
    }
}

bool PlayCardsPacket::operator<<(UdpBuffer &buffer) {
    try {
        buffer >> transaction_id_ >> player_index >> cards;
        return true;
    } catch (...) {
        return false;
    }
}