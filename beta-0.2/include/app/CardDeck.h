//
// Created by apple on 2020/4/23.
//

#ifndef POKER_CARDDECK_H
#define POKER_CARDDECK_H

#include <set>
#include "protocol/UdpBuffer.h"

enum CardRank {
    ANY,
    THREE = 3,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    TWO,
    BLACK_JOKER,
    RED_JOKER
};

enum Suit {
    HEART,
    SPADE,
    DIAMOND,
    CLUB,
    JOKER
};

struct Card {
    Card() : rank(THREE), suit(HEART) {}

    Card(CardRank rank, Suit suit) : rank(rank), suit(suit) {}

    bool operator<(const Card& rhs) const {
        if (rank == rhs.rank) {
            return suit < rhs.suit;
        } else {
            return rank < rhs.rank;
        }
    }

    CardRank rank;
    Suit suit;
};

class CardDeck {
public:
    CardDeck();

    void Fill(int num_deck);

    std::multiset<Card> Draw(int num_cards);

    std::multiset<Card> GetRemainingCards();

private:
    Card* cards;
    int remaining_cards;
};


UdpBuffer& operator<<(UdpBuffer& buffer, const Card& card);
UdpBuffer& operator>>(UdpBuffer& buffer, Card& card);

#endif //POKER_CARDDECK_H
