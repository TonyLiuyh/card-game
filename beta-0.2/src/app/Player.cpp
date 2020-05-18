//
// Created by apple on 2020/4/23.
//

#include "app/Player.h"

UdpBuffer& operator<<(UdpBuffer& buffer, const Player& player) {
    buffer << player.endpoint << player.name
           << player.host_name << (uint8_t)player.status << player.is_robot;
    return buffer;
}

UdpBuffer& operator>>(UdpBuffer& buffer, Player& player) {
    uint8_t status;
    buffer >> player.endpoint >> player.name >> player.host_name >> status >> player.is_robot;
    player.status = static_cast<Player::Status>(status);
    return buffer;
}

RoomInfo Room::GenerateInfo() const {
    if (status == NONE) {
        return RoomInfo();
    } else {
        return RoomInfo(room_name, players.size(), status, players[0]);
    }
}

bool Room::DoAction(const boost::asio::ip::udp::endpoint &endpoint, Action action) {
    if (action == JOIN) {
        return Join(endpoint);
    } else if (action == LEAVE) {
        return Leave(endpoint);
    } else if (action == READY) {
        return Ready(endpoint);
    } else if (action == CANCEL) {
        return Cancel(endpoint);
    } else if (action == MESSAGE) {
        return true;
    } else{
        return false;
    }
}

bool Room::Join(const boost::asio::ip::udp::endpoint &endpoint) {
    if (players.size() < 3) {
        players.push_back(endpoint);
        ready.push_back(endpoint == boost::asio::ip::udp::endpoint());
        return true;
    } else {
        return false;
    }
}

bool Room::Leave(const boost::asio::ip::udp::endpoint &endpoint) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i] == endpoint) {
            players.erase(players.begin() + i);
            ready.erase(ready.begin() + i);

            for (int j = 0; j < players.size(); ++j) {
                if (players[j] != boost::asio::ip::udp::endpoint()) {
                    std::swap(players[0], players[j]);
                    std::swap(ready[0], ready[j]);
                    ready[0] = true;
                    return true;
                }
            }
            players.clear();
            ready.clear();
            return true;
        }
    }
    return false;
}

bool Room::Ready(const boost::asio::ip::udp::endpoint &endpoint) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i] == endpoint) {
            ready[i] = true;
            return true;
        }
    }
    return false;
}

bool Room::Cancel(const boost::asio::ip::udp::endpoint &endpoint) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i] == endpoint) {
            ready[i] = false;
            return true;
        }
    }
    return false;
}

bool Room::AllReady() const {
    for (auto && i : ready) {
        if (!i) {
            return false;
        }
    }
    return ready.size() == 3;
}

UdpBuffer& operator<<(UdpBuffer& buffer, const Room& room) {
    buffer << room.room_name << room.players << room.ready
        << (uint8_t)room.status << room.this_player_index;
    return buffer;
}

UdpBuffer& operator>>(UdpBuffer& buffer, Room& room) {
    uint8_t status;
    buffer >> room.room_name >> room.players >> room.ready
        >> status >> room.this_player_index;
    room.status = static_cast<Room::Status>(status);
    return buffer;
}

UdpBuffer& operator<<(UdpBuffer& buffer, const RoomInfo& room_info) {
    buffer << room_info.room_name << room_info.player_num
        << (uint8_t)room_info.status << room_info.host_endpoint;
    return buffer;
}

UdpBuffer& operator>>(UdpBuffer& buffer, RoomInfo& room_info) {
    uint8_t status;
    buffer >> room_info.room_name >> room_info.player_num >> status >> room_info.host_endpoint;
    room_info.status = static_cast<Room::Status>(status);
    return buffer;
}