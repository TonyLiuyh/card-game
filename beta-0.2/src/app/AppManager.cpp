//
// Created by apple on 2020/5/10.
//

#include "app/AppManager.h"
#include "app/InputHandler.h"
#include "app/MainMenu.h"
#include "app/MultiplayerScene.h"
#include "app/RoomScene.h"
#include "app/GameScene.h"
#include <curses.h>
#include <iostream>

AppManager::ptr AppManager::instance = nullptr;

AppManager::ptr AppManager::Instance() {
    if (instance == nullptr) {
        instance.reset(new AppManager());
    }
    return instance;
}

AppManager::AppManager()
    : io_service()
    , packet_manager(io_service)
    , this_player()
    , discovered_players()
    , broadcast_timer(io_service, 5000, [this] { Broadcast(); }, -1)
    , remove_inactive_players_timer(io_service, 5000, [this] { RemoveInactivePlayers(); }, -1)
    , resend_timers()
    , current_room() {
    char buffer[50];
    gethostname(buffer, 50);
    this_player.host_name = std::string(buffer);
    this_player.name = this_player.host_name;
    this_player.is_robot = false;
}

void AppManager::Start() {
    InputHandler::Initialize();

    // make sure that the terminal is large enough to draw everything correctly
    int x = InputHandler::ScreenSizeX(), y = InputHandler::ScreenSizeY();
    if (x < 40 || y < 120) {
        endwin();
        std::cout << "Please make sure your terminal size is at least 40 x 120\r\n";
        std::cout << "Current size: " << x << " x " << y << "\r\n";
        return;
    }

    int i;
    for (i = 9580; i < 9583; ++i) {
        if (packet_manager.Listen(i)) {
            break;
        }
    }

    if (i == 9583) {
        endwin();
        std::cout << "One instance of this game is already running\r\n";
        return;
    }
    packet_manager.RegisterPacket<HandshakePacket>(
            [this](const HandshakePacket& packet) { OnHandshakePacket(packet); });
    packet_manager.RegisterPacket<OfflinePacket>(
            [this](const OfflinePacket& packet) { OnOfflinePacket(packet); });
    packet_manager.RegisterPacket<RoomInfoPacket>(
            [this](const RoomInfoPacket& packet) { OnRoomInfoPacket(packet); });
    packet_manager.RegisterPacket<RoomPacket>(
            [this](const RoomPacket& packet) { OnRoomPacket(packet); });
    packet_manager.RegisterPacket<AcknowledgePacket>(
            [this](const AcknowledgePacket& packet) { OnAcknowledgementPacket(packet); });
    packet_manager.RegisterPacket<StartGamePacket>(
            [this](const StartGamePacket& packet) { OnStartGamePacket(packet); });
    packet_manager.RegisterPacket<PlayCardsPacket>(
            [this](const PlayCardsPacket& packet) { OnPlayCardsPacket(packet); });


    this_player.endpoint = packet_manager.GetLocalEndpoint();


    boost::asio::ip::udp::resolver resolver(io_service);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::host_name(), "");
    auto iter = resolver.resolve(query);
    boost::asio::ip::udp::resolver::iterator end;
    while (iter != end) {
        if (iter->endpoint().address().is_v4() &&
            iter->endpoint().address().to_string() != "127.0.0.1") {
            this_player.endpoint.address(iter->endpoint().address());
            break;
        }
        ++iter;
    }
    this_player.status = Player::ONLINE;
    Broadcast();
    broadcast_timer.Start();
    remove_inactive_players_timer.Start();

    InputHandler::Instance()->Start();
    MainMenu::Instance()->Open();
    io_service.run();
    InputHandler::Instance()->Run();
}

void AppManager::Stop() {
    boost::asio::ip::udp::endpoint broadcast_endpoint(boost::asio::ip::address_v4::broadcast(), 9580);
    OfflinePacket packet(broadcast_endpoint);
    for (int i = 9580; i < 9583; ++i) {
        packet.endpoint_.port(i);
        packet_manager.Send(packet, false);
    }
    io_service.stop();
}

void AppManager::CreateRoom(const std::string &name) {
    current_room = Room(name, this_player.endpoint);
    this_player.status = Player::IN_ROOM;
    SendRoomInfoUpdatePackets();
    Broadcast();
    broadcast_timer.Restart();
    RoomScene::Instance()->UpdateRoom(current_room, Room::JOIN, this_player.endpoint);
    MultiplayerScene::Instance()->Close();
    RoomScene::Instance()->Open();
}

void AppManager::RequestJoinRoom(const boost::asio::ip::udp::endpoint &endpoint) {
    RoomPacket packet(endpoint, Room::JOIN);
    packet_manager.Send(packet);
}

void AppManager::RequestRoomAction(Room::Action action) {
    if (current_room.this_player_index == 0) {
        current_room.DoAction(this_player.endpoint, action);
        SendRoomUpdatePackets(action, this_player.endpoint);
        if (action == Room::LEAVE) {
            if (!current_room.players.empty()) {
                rooms.emplace(current_room.players[0], current_room.GenerateInfo());
                MultiplayerScene::Instance()->UpdateRooms(rooms);
            }
            current_room = Room();
            SendRoomInfoUpdatePackets();
            RoomScene::Instance()->Close();
            RoomScene::Instance()->ClearMessages();
            MultiplayerScene::Instance()->Open();
            this_player.status = Player::ONLINE;
            Broadcast();
            broadcast_timer.Restart();
        }
    } else {
        RoomPacket packet(current_room.players[0], action);
        packet_manager.Send(packet);
    }
}

void AppManager::KickPlayer(int index) {
    if (current_room.this_player_index == 0 && current_room.status != Room::NONE) {
        if (index < current_room.players.size()) {
            boost::asio::ip::udp::endpoint endpoint = current_room.players[index];
            current_room.DoAction(endpoint, Room::LEAVE);
            SendRoomUpdatePackets(Room::LEAVE, endpoint);
            SendRoomInfoUpdatePackets();
            RoomScene::Instance()->UpdateRoom(current_room, Room::LEAVE, endpoint);
        } else {
            boost::asio::ip::udp::endpoint robot_endpoint;
            current_room.DoAction(robot_endpoint, Room::JOIN);
            SendRoomUpdatePackets(Room::JOIN, robot_endpoint);
            SendRoomInfoUpdatePackets();
            RoomScene::Instance()->UpdateRoom(current_room, Room::JOIN, robot_endpoint);
        }
    }
}

void AppManager::SendMessage(const std::string& message) {
    if (current_room.this_player_index == 0) {
        RoomScene::Instance()->UpdateRoom(current_room, Room::MESSAGE, this_player.endpoint, message);
        SendRoomUpdatePackets(Room::MESSAGE, this_player.endpoint, message);
    } else {
        RoomPacket packet(current_room.players[0], Room::MESSAGE, message);
        packet_manager.Send(packet);
    }
}

Player AppManager::GetPlayer(const boost::asio::ip::udp::endpoint &endpoint) {
    if (endpoint == this_player.endpoint) {
        return this_player;
    } else if (discovered_players.find(endpoint) != discovered_players.end()) {
        return discovered_players[endpoint];
    } else {
        return Player();
    }
}

boost::asio::io_service & AppManager::GetIoService() {
    return io_service;
}

bool AppManager::StartGame() {
    if (!current_room.AllReady()) {
        return false;
    }

    if (current_room.status != Room::NONE && current_room.this_player_index == 0) {
        CardDeck deck;
        deck.Fill(1);
        std::vector<std::vector<Card>> cards(3);
        int first = 0;
        for (int i = 0; i < 3; ++i) {
            std::multiset<Card> one_player(deck.Draw(18));
            cards[i] = std::vector<Card>(one_player.begin(), one_player.end());
            if (one_player.find(Card(THREE, HEART)) != one_player.end()) {
                first = i;
            }
        }
        RoomScene::Instance()->Close();

        std::vector<Player> players_info(3);
        for (int i = 0; i < current_room.players.size(); ++i) {
            players_info[i] = GetPlayer(current_room.players[i]);
        }
        GameScene::Instance()->Initialize(players_info, cards, 0, first);
        GameScene::Instance()->Open();
        GameScene::Instance()->Start();
        for (int i = 1; i < current_room.players.size(); ++i) {
            StartGamePacket packet(current_room.players[i], cards, first);
            packet_manager.Send(packet);
            EnsureAcknowledgement(packet);
        }
        current_room.status = Room::IN_GAME;
        for (int i = 1; i < current_room.players.size(); ++i) {
            if (current_room.players[i] != boost::asio::ip::udp::endpoint()) {
                current_room.ready[i] = false;
            }
        }
        SendRoomInfoUpdatePackets();

        this_player.status = Player::IN_GAME;
        Broadcast();
        broadcast_timer.Restart();

        return true;
    } else {
        return false;
    }
}

void AppManager::PlayCards(const std::multiset<Card> &cards) {
    if (current_room.this_player_index == 0) {
        for (int i = 1; i < current_room.players.size(); ++i) {
            PlayCardsPacket packet(current_room.players[i], cards, 0);
            packet_manager.Send(packet);
            EnsureAcknowledgement(packet);
        }
        GameScene::Instance()->PlayCards(0, cards);
    } else {
        PlayCardsPacket packet(current_room.players[0], cards, current_room.this_player_index);
        packet_manager.Send(packet);
    }
}

void AppManager::PlayCards(const std::multiset<Card> &cards, int index) {
    if (current_room.this_player_index == 0) {
        for (int i = 1; i < current_room.players.size(); ++i) {
            if (current_room.players[i] != boost::asio::ip::udp::endpoint()) {
                PlayCardsPacket packet(current_room.players[i], cards, index);
                packet_manager.Send(packet);
                EnsureAcknowledgement(packet);
            }
        }
        GameScene::Instance()->PlayCards(index, cards);
    } else {
        PlayCardsPacket packet(current_room.players[0], cards, index);
        packet_manager.Send(packet);
    }
}

void AppManager::FinishGame(bool again) {
    GameScene::Instance()->Close();
    if (again) {
        current_room.status = Room::OPEN;
        this_player.status = Player::IN_ROOM;
        RoomScene::Instance()->UpdateRoom(current_room, Room::UPDATE, this_player.endpoint);
        RoomScene::Instance()->Open();
        if (current_room.this_player_index == 0) {
            SendRoomInfoUpdatePackets();
        }
    } else {
        this_player.status = Player::ONLINE;
        RequestRoomAction(Room::LEAVE);
        MultiplayerScene::Instance()->Open();
    }
    Broadcast();
    broadcast_timer.Restart();
}

void AppManager::OnHandshakePacket(const HandshakePacket &packet) {
    if (packet.endpoint_ == this_player.endpoint) {
        return;
    }
    if (packet.is_request_) {
        HandshakePacket response(packet.endpoint_, this_player);
        response.is_request_ = false;
        packet_manager.Send(response);
    }
    time_t now = time(nullptr);
    auto iter = discovered_players.find(packet.endpoint_);
    if (iter != discovered_players.end()) {
        iter->second.last_active_time = now;
        if (iter->second.status != static_cast<Player::Status>(packet.status)) {
            iter->second.status = static_cast<Player::Status>(packet.status);
            MultiplayerScene::Instance()->UpdatePlayers(discovered_players);
        }
    } else {
        discovered_players.emplace(packet.endpoint_,
                Player(packet.endpoint_, packet.name, packet.host_name, now, static_cast<Player::Status>(packet.status)));
        MultiplayerScene::Instance()->UpdatePlayers(discovered_players);
        if (current_room.status != Room::NONE && current_room.this_player_index == 0) {
            RoomInfoPacket room_info_packet(packet.endpoint_, current_room.GenerateInfo());
            packet_manager.Send(room_info_packet);
        }
    }
}

void AppManager::OnOfflinePacket(const OfflinePacket& packet) {
    auto iter = discovered_players.find(packet.endpoint_);
    if (iter != discovered_players.end()) {
        discovered_players.erase(iter);
        MultiplayerScene::Instance()->UpdatePlayers(discovered_players);
    }
}

void AppManager::OnRoomInfoPacket(const RoomInfoPacket &packet) {
    auto iter = rooms.find(packet.endpoint_);
    AcknowledgePacket acknowledgement(packet.endpoint_, packet.transaction_id_);
    packet_manager.Send(acknowledgement);
    if (iter == rooms.end()) {
        if (packet.room.status != Room::NONE) {
            rooms.emplace(packet.endpoint_, packet.room);
            MultiplayerScene::Instance()->UpdateRooms(rooms);
        }
    } else {
        if (packet.room.status != Room::NONE) {
            iter->second = packet.room;
        } else {
            rooms.erase(iter);
        }
        MultiplayerScene::Instance()->UpdateRooms(rooms);
    }
}

void AppManager::OnRoomPacket(const RoomPacket &packet) {
    auto iter = discovered_players.find(packet.endpoint_);
    if (iter == discovered_players.end()) {
        return;
    }
    Player& player = iter->second;
    if (current_room.this_player_index == 0 && packet.is_request_ && current_room.status != Room::NONE) {
        if (current_room.DoAction(player.endpoint, packet.action)) {
            SendRoomUpdatePackets(packet.action, player.endpoint, packet.message);
            if (packet.action == Room::JOIN) {
                player.status = Player::IN_ROOM;
                MultiplayerScene::Instance()->UpdatePlayers(discovered_players);
            } else if (packet.action == Room::LEAVE) {
                player.status = Player::ONLINE;
                MultiplayerScene::Instance()->UpdatePlayers(discovered_players);

                if (current_room.status == Room::IN_GAME) {
                    GameScene::Instance()->PlayerLeave(player.endpoint);
                }
            }
            RoomScene::Instance()->UpdateRoom(current_room, packet.action, player.endpoint, packet.message);
            if (packet.action == Room::JOIN || packet.action == Room::LEAVE) {
                SendRoomInfoUpdatePackets();
            }
        } else {
            RoomPacket response(player.endpoint, packet.action, current_room, "Invalid action");
            packet_manager.Send(response);
        }
    } else if (!packet.is_request_ && (current_room.this_player_index != 0 || current_room.status == Room::NONE)) {
        AcknowledgePacket acknowledgement(packet.endpoint_, packet.transaction_id_);
        packet_manager.Send(acknowledgement);
        if (packet.success) {
            current_room = packet.room;
            if (packet.action_endpoint == this_player.endpoint) {
                if (packet.action == Room::JOIN) {
                    this_player.status = Player::IN_ROOM;
                    MultiplayerScene::Instance()->Close();
                    RoomScene::Instance()->Open();
                    RoomScene::Instance()->UpdateRoom(current_room, packet.action, packet.action_endpoint);
                    Broadcast();
                    broadcast_timer.Restart();
                } else if (packet.action == Room::LEAVE) {
                    this_player.status = Player::ONLINE;
                    current_room = Room();
                    RoomScene::Instance()->Close();
                    RoomScene::Instance()->ClearMessages();
                    MultiplayerScene::Instance()->Open();
                    Broadcast();
                    broadcast_timer.Restart();
                } else if (packet.action == Room::MESSAGE) {
                    RoomScene::Instance()->UpdateRoom(current_room, packet.action, packet.action_endpoint, packet.message);
                } else {
                    RoomScene::Instance()->UpdateRoom(current_room, packet.action, packet.action_endpoint);
                }
                refresh();
            } else {
                RoomScene::Instance()->UpdateRoom(current_room, packet.action, packet.action_endpoint, packet.message);
                if (current_room.this_player_index == 0) {
                    SendRoomInfoUpdatePackets();
                }
                if (current_room.status == Room::IN_GAME && packet.action == Room::LEAVE) {
                    GameScene::Instance()->PlayerLeave(packet.action_endpoint);
                }
            }
        } else {
            RoomScene::Instance()->UpdateRoom(current_room, Room::UPDATE, packet.action_endpoint);
            if (packet.action == Room::JOIN) {
                MultiplayerScene::Instance()->FailToJoin(packet.room);
            } else {
                RoomScene::Instance()->ActionFailed(packet.action, packet.message);
            }
        }
    }
}

void AppManager::OnAcknowledgementPacket(const AcknowledgePacket& packet) {
    auto iter = resend_timers.find(packet.transaction);
    if (iter != resend_timers.end()) {
        iter->second.Stop();
        resend_timers.erase(iter);
    }
}

void AppManager::OnStartGamePacket(const StartGamePacket &packet) {
    if (current_room.status != Room::NONE && current_room.this_player_index != 0) {
        RoomScene::Instance()->Close();
        std::vector<Player> players_info(3);
        for (int i = 0; i < current_room.players.size(); ++i) {
            players_info[i] = GetPlayer(current_room.players[i]);
        }
        GameScene::Instance()->Initialize(players_info, packet.cards, current_room.this_player_index,
                packet.first_player);
        GameScene::Instance()->Open();
        GameScene::Instance()->Start();
        AcknowledgePacket acknowledge(packet.endpoint_, packet.transaction_id_);
        packet_manager.Send(acknowledge);

        this_player.status = Player::IN_GAME;
        for (int i = 1; i < current_room.players.size(); ++i) {
            if (current_room.players[i] != boost::asio::ip::udp::endpoint()) {
                current_room.ready[i] = false;
            }
        }

        Broadcast();
        broadcast_timer.Restart();
    }
}

void AppManager::OnPlayCardsPacket(const PlayCardsPacket &packet) {
    if (current_room.status != Room::NONE && current_room.this_player_index != 0) {
        GameScene::Instance()->PlayCards(packet.player_index, packet.cards);
        AcknowledgePacket acknowledge(packet.endpoint_, packet.transaction_id_);
        packet_manager.Send(acknowledge);
    } else if (current_room.status != Room::NONE && current_room.this_player_index == 0) {
        GameScene::Instance()->PlayCards(packet.player_index, packet.cards);
        for (int i = 1; i < current_room.players.size(); ++i) {
            PlayCardsPacket response(current_room.players[i], packet.cards, packet.player_index);
            packet_manager.Send(response);
            EnsureAcknowledgement(response);
        }
    }
}

void AppManager::Broadcast() {
    boost::asio::ip::udp::endpoint broadcast_endpoint(boost::asio::ip::address_v4::broadcast(), 9580);
    for (int i = 9580; i < 9583; ++i) {
        broadcast_endpoint.port(i);
        HandshakePacket packet(broadcast_endpoint, this_player);
        packet.is_request_ = true;
        packet_manager.Send(packet);
    }
}

void AppManager::RemoveInactivePlayers() {
    bool removed = false;
    time_t now = time(nullptr);
    if (current_room.status != Room::NONE && current_room.this_player_index == 0) {
        for (int i = 1; i < current_room.players.size(); ++i) {
            if (current_room.players[i] != boost::asio::ip::udp::endpoint() &&
                    now - GetPlayer(current_room.players[i]).last_active_time > 10) {
                boost::asio::ip::udp::endpoint left_player = current_room.players[i];
                current_room.DoAction(left_player, Room::LEAVE);
                RoomScene::Instance()->UpdateRoom(current_room, Room::LEAVE, left_player);
                SendRoomUpdatePackets(Room::LEAVE, left_player);
                discovered_players.erase(left_player);
                SendRoomInfoUpdatePackets();
                removed = true;
            }
        }
    } else if (current_room.status != Room::NONE && current_room.this_player_index == 1) {
        if (now - GetPlayer(current_room.players[0]).last_active_time > 10) {
            boost::asio::ip::udp::endpoint left_host = current_room.players[0];
            current_room.DoAction(left_host, Room::LEAVE);
            SendRoomUpdatePackets(Room::LEAVE, left_host);
            current_room.this_player_index = 0;
            RoomScene::Instance()->UpdateRoom(current_room, Room::LEAVE, left_host);
            SendRoomInfoUpdatePackets();
            discovered_players.erase(left_host);
            rooms.erase(left_host);
            MultiplayerScene::Instance()->UpdateRooms(rooms);
            removed = true;
        }
    }
    for (auto iter = discovered_players.begin(); iter != discovered_players.end(); ) {
        if (now - iter->second.last_active_time > 10) {
            if (rooms.find(iter->second.endpoint) != rooms.end()) {
                rooms.erase(iter->second.endpoint);
                MultiplayerScene::Instance()->UpdateRooms(rooms);
            }
            discovered_players.erase(iter++);
            removed = true;
        } else {
            ++iter;
        }
    }
    if (removed) {
        MultiplayerScene::Instance()->UpdatePlayers(discovered_players);
    }
}

void AppManager::SendRoomUpdatePackets(Room::Action action, const boost::asio::ip::udp::endpoint& endpoint,
        const std::string& message) {
    RoomPacket packet(boost::asio::ip::udp::endpoint(), action, current_room, endpoint,
            message);
    for (int i = 0; i < current_room.players.size(); ++i) {
        if (current_room.players[i] != this_player.endpoint) {
            packet.endpoint_ = current_room.players[i];
            packet.room.this_player_index = i;
            packet_manager.Send(packet);
            EnsureAcknowledgement(packet);
        }
    }
    if (action == Room::LEAVE && endpoint != this_player.endpoint) {
        packet.endpoint_ = endpoint;
        packet_manager.Send(packet);
        EnsureAcknowledgement(packet);
    }
}

void AppManager::SendRoomInfoUpdatePackets() {
    RoomInfoPacket packet(boost::asio::ip::udp::endpoint(), current_room.GenerateInfo());
    for (auto& p : discovered_players) {
        packet.endpoint_ = p.second.endpoint;
        packet_manager.Send(packet);
        EnsureAcknowledgement(packet);
    }
}