//
// Created by apple on 2020/5/13.
//

#ifndef CARD_GAME_ROOMSCENE_H
#define CARD_GAME_ROOMSCENE_H

#include "app/Scene.h"
#include "app/Player.h"
#include "app/Button.h"
#include "app/TextLabel.h"
#include "app/InputField.h"
#include <memory>
#include <vector>

class RoomScene : public Scene {
public:
    typedef std::shared_ptr<RoomScene> ptr;

    static ptr Instance();

    void UpdateRoom(const Room& room, Room::Action action,
            const boost::asio::ip::udp::endpoint& endpoint, const std::string& msg = "");

    void ActionFailed(Room::Action action, const std::string& message);

    void ClearMessages();

private:
    RoomScene();

    void Leave();

    void Kick(int index);

    void Ready();

    void Start();

private:
    static ptr instance;

    Button::ptr leave_button;
    Button::ptr ready_button;
    Button::ptr start_button;
    TextLabel::ptr messages_label;
    std::vector<TextLabel::ptr> player_labels;
    std::vector<Button::ptr> kick_buttons;
    std::vector<std::string> messages;
    InputField::ptr input_field;

    bool ready;
};

#endif //CARD_GAME_ROOMSCENE_H
