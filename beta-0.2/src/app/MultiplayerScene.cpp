//
// Created by apple on 2020/5/11.
//

#include "app/MultiplayerScene.h"
#include "app/MainMenu.h"
#include "app/AppManager.h"
#include "app/RoomScene.h"
#include <curses.h>

MultiplayerScene::ptr MultiplayerScene::instance = nullptr;

MultiplayerScene::ptr MultiplayerScene::Instance() {
    if (instance == nullptr) {
        instance.reset(new MultiplayerScene());
    }
    return instance;
}

MultiplayerScene::MultiplayerScene()
    : Scene(1, 7)
    , players()
    , rooms()
    , back_button(Button::Create("Back", [this] { Back(); }, 1, 5))
    , create_room_button(Button::Create("Create Room", [this] { CreateRoom(); }, 30, 60))
    , online_players_label(TextLabel::Create("Online Players", 8, 20, 1, 14))
    , rooms_label(TextLabel::Create("Rooms", 8, 75, 1, 5))
    , player_labels()
    , room_labels()
    , join_buttons() {
    canvas.AddComponent(back_button, 0, 0);
    canvas.AddComponent(create_room_button, 6, 0);
    canvas.AddComponent(online_players_label);
    canvas.AddComponent(rooms_label);
    for (int i = 0; i < 5; ++i) {
        player_labels.push_back(TextLabel::Create("", 11 + 3 * i, 30, 2, 40, TextLabel::LEFT));
        room_labels.push_back(TextLabel::Create("", 11 + 3 * i, 80, 2, 40, TextLabel::LEFT));
        join_buttons.push_back(Button::Create("Join", [this, i] { Join(i); }, 10 + 3 * i, 110));
        canvas.AddComponent(player_labels[i]);
        canvas.AddComponent(room_labels[i]);
        canvas.AddComponent(join_buttons[i], i + 1, 0);
        join_buttons[i]->SetActive(false);
    }
}

void MultiplayerScene::UpdatePlayers(const std::map<boost::asio::ip::udp::endpoint, Player> &new_players) {
    players = new_players;
    auto iter = players.begin();
    for (auto & player_label : player_labels) {
        if (iter != players.end()) {
            //std::string str = iter->second.endpoint.address().to_string() + ":" +
            //        std::to_string(iter->second.endpoint.port()) + "\n" + iter->second.name + " ";
            std::string str = iter->second.name + "\n";
            if (iter->second.status == Player::ONLINE) {
                str += "\bGOnline";
            } else if (iter->second.status == Player::IN_ROOM) {
                str += "\bGIn Room";
            } else if (iter->second.status == Player::IN_GAME) {
                str += "\bLIn Game";
            } else {
                str += std::to_string((uint8_t)iter->second.status);
            }
            player_label->SetText(str);
            ++iter;
        } else {
            player_label->SetText("");
        }
    }
    if (active) {
        for (auto& label : player_labels) {
            label->Draw();
        }
        refresh();
    }
}

void MultiplayerScene::UpdateRooms(const std::map<boost::asio::ip::udp::endpoint, RoomInfo> &new_rooms) {
    rooms = new_rooms;
    auto iter = rooms.begin();
    for (int i = 0; i < room_labels.size(); ++i) {
        if (iter != rooms.end()) {
            std::string str = iter->second.room_name + "\n" +
                    std::to_string(iter->second.player_num) + " / 3  ";
            if (iter->second.status == Room::OPEN) {
                if (iter->second.player_num == 3) {
                    join_buttons[i]->SetActive(false);
                    str += "Full";
                } else {
                    join_buttons[i]->SetActive(true);
                    str += "Open";
                }
            } else if (iter->second.status == Room::IN_GAME) {
                join_buttons[i]->SetActive(false);
                str += "In Game";
            }
            room_labels[i]->SetText(str);
            ++iter;
        } else {
            room_labels[i]->SetText("");
            join_buttons[i]->SetActive(false);
        }
    }
    if (active) {
        for (int i = 0; i < room_labels.size(); ++i) {
            room_labels[i]->Draw();
            if (join_buttons[i]->Active()) {
                join_buttons[i]->Draw();
            } else {
                join_buttons[i]->Clear();
            }
        }
        refresh();
    }
}

void MultiplayerScene::JoinRoom(const Room &room) {
    Close();
    RoomScene::Instance()->Open();
}

void MultiplayerScene::FailToJoin(const Room &room) {
    std::string msg = "Failed to join " + room.room_name;
    mvaddstr(0, 0, msg.c_str());
}

void MultiplayerScene::Back() {
    Close();
    MainMenu::Instance()->Open();
}

void MultiplayerScene::CreateRoom() {
    AppManager::Instance()->CreateRoom("My Room");
}

void MultiplayerScene::Join(int room) {
    auto iter = rooms.begin();
    for (int i = 0; i < room; ++i) {
        ++iter;
    }
    AppManager::Instance()->RequestJoinRoom(iter->first);
}
