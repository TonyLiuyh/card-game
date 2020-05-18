//
// Created by apple on 2020/5/10.
//

#ifndef CARD_GAME_APPMANAGER_H
#define CARD_GAME_APPMANAGER_H

#include <memory>
#include "protocol/PacketManager.h"
#include "app/Player.h"
#include "tool/Timer.h"
#include "app/CardDeck.h"
#include <vector>
#include <unordered_map>

class AppManager {
public:
    typedef std::shared_ptr<AppManager> ptr;

    static ptr Instance();

    void Start();

    void Stop();

    void CreateRoom(const std::string& name);

    void RequestJoinRoom(const boost::asio::ip::udp::endpoint& endpoint);

    void RequestRoomAction(Room::Action action);

    void KickPlayer(int index);

    void SendMessage(const std::string& message);

    Player GetPlayer(const boost::asio::ip::udp::endpoint& endpoint);

    boost::asio::io_service& GetIoService();

    void StartSinglePlayerGame();

    bool StartGame();

    void PlayCards(const std::multiset<Card>& cards);

    void PlayCards(const std::multiset<Card>& cards, int index);

    void FinishGame(bool again);

private:
    AppManager();

    void OnHandshakePacket(const HandshakePacket& packet);

    void OnOfflinePacket(const OfflinePacket& packet);

    void OnRoomInfoPacket(const RoomInfoPacket& packet);

    void OnRoomPacket(const RoomPacket& packet);

    void OnAcknowledgementPacket(const AcknowledgePacket& packet);

    void OnStartGamePacket(const StartGamePacket& packet);

    void OnPlayCardsPacket(const PlayCardsPacket& packet);

    void Broadcast();

    void RemoveInactivePlayers();

    void SendRoomUpdatePackets(Room::Action action, const boost::asio::ip::udp::endpoint& endpoint,
            const std::string& message = "");

    void SendRoomInfoUpdatePackets();

    template <typename PacketType>
    void EnsureAcknowledgement(const PacketType& packet) {
        resend_timers.emplace(packet.transaction_id_, Timer(io_service, 500, [this, packet] {
            packet_manager.Send(packet);
            auto iter = resend_timers.find(packet.transaction_id_);
            if (iter->second.GetExpiredTimes() == 3) {
                iter->second.Stop();
                resend_timers.erase(iter);
            }
        }, 3));
        resend_timers.find(packet.transaction_id_)->second.Start();
    }

private:
    static ptr instance;

    boost::asio::io_service io_service;

    PacketManager packet_manager;

    Player this_player;

    Timer broadcast_timer;

    Timer remove_inactive_players_timer;

    std::map<boost::asio::ip::udp::endpoint, Player> discovered_players;

    // from room host endpoint to room
    std::map<boost::asio::ip::udp::endpoint, RoomInfo> rooms;

    std::unordered_map<uint32_t, Timer> resend_timers;

    Room current_room;
};

#endif //CARD_GAME_APPMANAGER_H
