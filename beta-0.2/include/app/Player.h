//
// Created by apple on 2020/4/23.
//

#ifndef POKER_PLAYER_H
#define POKER_PLAYER_H

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include "protocol/UdpBuffer.h"

struct Player {
    enum Status {
        NONE,
        ONLINE,
        IN_ROOM,
        IN_GAME
    };

    boost::asio::ip::udp::endpoint endpoint;
    std::string name;
    std::string host_name;
    time_t last_active_time;
    Status status;
    bool is_robot;

    Player() : endpoint(), name("Robot"), host_name(), last_active_time(0), status(NONE), is_robot(true) {}

    Player(boost::asio::ip::udp::endpoint endpoint, std::string name,
            std::string host_name, time_t last_active_time, Status status)
        : endpoint(std::move(endpoint))
        , name(std::move(name))
        , host_name(std::move(host_name))
        , last_active_time(last_active_time)
        , status(status)
        , is_robot(false) {}
};

UdpBuffer& operator<<(UdpBuffer& buffer, const Player& player);

UdpBuffer& operator>>(UdpBuffer& buffer, Player& player);

struct RoomInfo;

class Room {
public:
    enum Status {
        NONE,
        OPEN,
        IN_GAME
    };

    enum Action {
        UPDATE,
        JOIN,
        LEAVE,
        READY,
        CANCEL,
        KICK,
        MESSAGE
    };

    std::string room_name;
    std::vector<boost::asio::ip::udp::endpoint> players;
    std::vector<bool> ready;
    Status status;
    uint8_t this_player_index;

    Room() : room_name(), players(), ready(), status(NONE), this_player_index(0) {}

    Room(std::string room_name, const boost::asio::ip::udp::endpoint& host_endpoint)
        : room_name(std::move(room_name))
        , players(1, host_endpoint)
        , ready(1, true)
        , status(OPEN)
        , this_player_index(0) {}

    RoomInfo GenerateInfo() const;

    bool DoAction(const boost::asio::ip::udp::endpoint& endpoint, Action action);

    bool AllReady() const;

private:
    bool Join(const boost::asio::ip::udp::endpoint& endpoint);

    bool Leave(const boost::asio::ip::udp::endpoint& endpoint);

    bool Ready(const boost::asio::ip::udp::endpoint& endpoint);

    bool Cancel(const boost::asio::ip::udp::endpoint& endpoint);
};

UdpBuffer& operator<<(UdpBuffer& buffer, const Room& room);

UdpBuffer& operator>>(UdpBuffer& buffer, Room& room);

struct RoomInfo {
    std::string room_name;
    uint8_t player_num;
    Room::Status status;
    boost::asio::ip::udp::endpoint host_endpoint;

    RoomInfo() : room_name(), player_num(0), status(Room::NONE), host_endpoint() {}

    RoomInfo(std::string room_name, int player_num, Room::Status status,
            boost::asio::ip::udp::endpoint host_endpoint)
        : room_name(std::move(room_name))
        , player_num(player_num)
        , status(status)
        , host_endpoint(std::move(host_endpoint)) {}
};

UdpBuffer& operator<<(UdpBuffer& buffer, const RoomInfo& room_info);

UdpBuffer& operator>>(UdpBuffer& buffer, RoomInfo& room_info);

#endif //POKER_PLAYER_H
