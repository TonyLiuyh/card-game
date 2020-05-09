//
// Created by apple on 2020/4/23.
//

#ifndef POKER_CARDGAME_H
#define POKER_CARDGAME_H

#include "app/CardDeck.h"
#include "app/Player.h"
#include <string>
//#include "protocol/Packets.h"
#include <vector>

struct PlayerInfo {
    std::string userName;
    uint32_t address;
    uint16_t port;
};

class CardPlayer {

};

class CardGameHost {
public:
    explicit CardGameHost(const std::vector<PlayerInfo> playersInfo);

    void Start();

    //void OnPacket(const Packet& packet);

private:

    bool PlayCards(const std::set<Card>& cards);

private:
    CardDeck deck;

    std::vector<CardPlayer> players;

    std::set<Card> cards;

    int self_index;
};

class CardGameClient {
public:
    explicit CardGameClient(const std::vector<PlayerInfo> playersInfo);

    void Start();

    //void OnPacket(const Packet& packet);

private:
    std::vector<CardPlayer> players;

    std::set<Card> cards;

    int self_index;
};

#endif //POKER_CARDGAME_H
