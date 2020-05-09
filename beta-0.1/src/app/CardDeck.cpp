//
// Created by apple on 2020/4/23.
//

#include "app/CardDeck.h"

CardDeck::CardDeck() : cards(nullptr), remaining_cards(0) {
    srand(time(nullptr));
}

void CardDeck::Fill(int num_deck) {
    delete cards;
    cards = new Card[num_deck * 54];
    for (int i = 0; i < num_deck; ++i) {
        int index = i * 54;
        for (int j = 3; j <= 15; ++j) {
            auto rank = static_cast<CardRank>(j);
            cards[index++] = Card(rank, SPADE);
            cards[index++] = Card(rank, HEART);
            cards[index++] = Card(rank, DIAMOND);
            cards[index++] = Card(rank, CLUB);
        }
        cards[index++] = Card(BLACK_JOKER, JOKER);
        cards[index] = Card(RED_JOKER, JOKER);
    }
    remaining_cards = num_deck * 54;
}

std::multiset<Card> CardDeck::Draw(int num_cards) {
    if (num_cards > remaining_cards) {
        num_cards = remaining_cards;
    }
    std::multiset<Card> result;
    for (int i = 0; i < num_cards; ++i) {
        int index = rand() % remaining_cards;
        result.emplace(cards[index]);
        if (index != remaining_cards - 1) {
            std::swap(cards[index], cards[remaining_cards - 1]);
        }
        --remaining_cards;
    }
    return result;
}

std::multiset<Card> CardDeck::GetRemainingCards() {
    std::multiset<Card> result(cards, cards + remaining_cards);
    remaining_cards = 0;
    return result;
}