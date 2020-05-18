//
// Created by apple on 2020/5/10.
//

#ifndef CARD_GAME_MULTIPLAYERSCENE_H
#define CARD_GAME_MULTIPLAYERSCENE_H

#include <boost/asio.hpp>
#include <memory>
#include <map>
#include "app/Scene.h"
#include "app/Player.h"
#include "app/Button.h"
#include "app/TextLabel.h"

class MultiplayerScene : public Scene {
public:
    typedef std::shared_ptr<MultiplayerScene> ptr;

    static ptr Instance();

    void UpdatePlayers(const std::map<boost::asio::ip::udp::endpoint, Player>& new_players);

    void UpdateRooms(const std::map<boost::asio::ip::udp::endpoint, RoomInfo>& new_rooms);

    void JoinRoom(const Room& room);

    void FailToJoin(const Room& room);

private:
    MultiplayerScene();

    void Back();

    void CreateRoom();

    void Join(int room);

private:
    static ptr instance;

    std::map<boost::asio::ip::udp::endpoint, Player> players;
    std::map<boost::asio::ip::udp::endpoint, RoomInfo> rooms;

    // UI
    Button::ptr back_button;
    Button::ptr create_room_button;
    TextLabel::ptr online_players_label;
    TextLabel::ptr rooms_label;
    std::vector<TextLabel::ptr> player_labels;
    std::vector<TextLabel::ptr> room_labels;
    std::vector<Button::ptr> join_buttons;
};

#endif //CARD_GAME_MULTIPLAYERSCENE_H
