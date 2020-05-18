//
// Created by apple on 2020/5/13.
//

#include "app/RoomScene.h"
#include "app/AppManager.h"
#include "app/MultiplayerScene.h"
#include <curses.h>

RoomScene::ptr RoomScene::instance = nullptr;

RoomScene::ptr RoomScene::Instance() {
    if (instance == nullptr) {
        instance.reset(new RoomScene());
    }
    return instance;
}

RoomScene::RoomScene()
    : Scene(2, 6)
    , leave_button(Button::Create("Leave Room", [this] { Leave(); }, 1, 8))
    , ready_button(Button::Create("Ready", [this] { Ready(); }, 38, 59))
    , start_button(Button::Create("Start", [this] { Start(); }, 38, 59))
    , messages_label(TextLabel::Create("", 19, 90, 20, 50, TextLabel::LEFT, TextLabel::BOT))
    , player_labels(3, nullptr)
    , kick_buttons(3, nullptr)
    , input_field(InputField::Create(30, 90, 50, [](const std::string& msg) { AppManager::Instance()->SendMessage(msg); }))
    , ready(false) {
    canvas.AddComponent(leave_button, 0, 0);
    canvas.AddComponent(ready_button, 5, 0);
    canvas.AddComponent(start_button, 5, 0);
    canvas.AddComponent(messages_label);
    canvas.AddComponent(input_field, 4, 0);
    ready_button->SetActive(false);
    start_button->SetActive(false);
    for (int i = 0; i < 3; ++i) {
        player_labels[i] = TextLabel::Create("", 10 + i * 3, 25, 1, 40, TextLabel::LEFT);
        kick_buttons[i] = Button::Create("Kick", [this, i] { Kick(i); }, 10 + i * 3, 52);
        kick_buttons[i]->SetActive(false);
        canvas.AddComponent(player_labels[i]);
        canvas.AddComponent(kick_buttons[i], i + 1, 0);
    }
}

void RoomScene::UpdateRoom(const Room &room, Room::Action action,
        const boost::asio::ip::udp::endpoint& endpoint, const std::string& msg) {
    std::string message;
    if (endpoint == room.players[room.this_player_index]) {
        message = "\bY" + AppManager::Instance()->GetPlayer(endpoint).name;
    } else {
        message = AppManager::Instance()->GetPlayer(endpoint).name;
    }

    if (action == Room::JOIN) {
        if (endpoint != boost::asio::ip::udp::endpoint()) {
            message += "\bE joined room";
        } else {
            message = "\bEA robot was added to room";
        }
        messages.push_back(message);
        messages_label->SetText(messages);
    } else if (action == Room::LEAVE) {
        if (endpoint != boost::asio::ip::udp::endpoint()) {
            message += "\bE left room";
        } else {
            message = "\bEA robot was removed from room";
        }
        messages.push_back(message);
        messages_label->SetText(messages);
    } else if (action == Room::MESSAGE) {
        message += "\bB: " + msg;
        messages.push_back(message);
        messages_label->SetText(messages);
    }

    for (int i = 0; i < room.players.size(); ++i) {
        Player player = AppManager::Instance()->GetPlayer(room.players[i]);
        std::string str;
        if (room.ready[i]) {
            str += "✓ ";
        } else {
            str += "  ";
        }
        if (room.this_player_index == i) {
            str += "\bY" + player.name + "\bB";
        } else {
            str += player.name;
        }

        if (i == 0) {
            str += " ♔";
        }
        player_labels[i]->SetText(str);
        if (room.this_player_index == 0 && i != 0) {
            kick_buttons[i]->SetActive(true);
            if (active) {
                kick_buttons[i]->Clear();
            }
            if (room.players[i] == boost::asio::ip::udp::endpoint()) {
                kick_buttons[i]->SetText("Remove Robot");
            } else {
                kick_buttons[i]->SetText("Kick Player");
            }
        } else {
            kick_buttons[i]->SetActive(false);
        }
    }
    for (int i = room.players.size(); i < 3; ++i) {
        player_labels[i]->SetText("  Waiting for player...");
        if (room.this_player_index == 0 && i == room.players.size()) {
            kick_buttons[i]->SetActive(true);
            if (active) {
                kick_buttons[i]->Clear();
            }
            kick_buttons[i]->SetText("Add Robot");
        } else {
            if (active) {
                kick_buttons[i]->Clear();
            }
            kick_buttons[i]->SetActive(false);
        }
    }
    start_button->SetActive(room.this_player_index == 0);
    if (room.this_player_index == 0 && active) {
        ready_button->Clear();
    }
    ready_button->SetActive(room.this_player_index != 0);
    if (room.this_player_index != 0) {
        if (room.ready[room.this_player_index]) {
            ready = true;
            ready_button->SetText("Cancel");
        } else {
            ready = false;
            if (active) {
                ready_button->Clear();
            }
            ready_button->SetText("Ready");
        }
    }

    if (active) {
        messages_label->Draw();
        for (int i = 0; i < 3; ++i) {
            player_labels[i]->Draw();
            kick_buttons[i]->Draw();
        }

        if (start_button->Active()) {
            start_button->Draw();
        } else {
            ready_button->Draw();
        }
        //input_field->Draw();
        InputField::AdjustCursor();
        refresh();
    }
}


void RoomScene::ActionFailed(Room::Action action, const std::string &message) {
    std::string str = "Failed to ";
    if (action == Room::READY) {
        str += "ready";
    } else if (action == Room::CANCEL) {
        str += "cancel";
    } else if (action == Room::LEAVE) {
        str += "leave";
    }
    str += ": " + message;
    mvaddstr(0, 30, str.c_str());
}

void RoomScene::ClearMessages() {
    messages.clear();
    messages_label->SetText("");
    input_field->Text("");
}

void RoomScene::Leave() {
    AppManager::Instance()->RequestRoomAction(Room::LEAVE);
}

void RoomScene::Ready() {
    if (!ready) {
        AppManager::Instance()->RequestRoomAction(Room::READY);
    } else {
        AppManager::Instance()->RequestRoomAction(Room::CANCEL);
    }
}

void RoomScene::Start() {
    if (!AppManager::Instance()->StartGame()) {
        mvaddstr(0, 0, "Players not ready");
        refresh();
    }
}

void RoomScene::Kick(int index) {
    AppManager::Instance()->KickPlayer(index);
}