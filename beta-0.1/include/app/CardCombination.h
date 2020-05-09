//
// Created by apple on 2020/4/25.
//

#ifndef POKER_CARDCOMBINATION_H
#define POKER_CARDCOMBINATION_H

#include <set>
#include <vector>
#include "app/CardDeck.h"
#include <map>
#include <set>
#include <unordered_map>

class CardCombination {
public:
    enum Type {
        PASS,
        INVALID,
        SINGLE,
        PAIR,
        THREE_OF_A_KIND,
        THREE_WITH_SINGLE,
        THREE_WITH_PAIR,
        STRAIGHT,
        STRAIGHT_PAIR,
        STRAIGHT_THREE,
        STRAIGHT_THREE_WITH_SINGLE,
        STRAIGHT_THREE_WITH_PAIR,
        FOUR_WITH_TWO,
        FOUR_WITH_TWO_PAIRS,
        NORMAL,
        FOUR_OF_A_KIND,
        DOUBLE_JOKER,
        FIVE_OF_A_KIND,
        SIX_OF_A_KIND,
        TRIPLE_JOKER,
        SEVEN_OF_A_KIND,
        EIGHT_OF_A_KIND,
        QUADRA_JOKER
    };

    CardCombination();

    CardCombination(Type type, CardRank main_rank, int length = 0);

    explicit CardCombination(const std::multiset<Card>& card_set);

    std::vector<Card> Display() const;

    Type GetType() const;

    CardRank Rank() const;

    bool operator<(const CardCombination& rhs) const;

    std::unordered_map<CardRank, int> GetHint(const std::multiset<Card>& all_cards) const;

private:
    void PositionCards();

    static std::pair<CardRank, int> FindLargerCards(std::map<CardRank, int>& card_map,
            int num, CardRank minimum_rank);

    static bool FindCards(std::map<CardRank, int>& card_map,
                                      int num, CardRank rank);

    std::unordered_map<CardRank, int> FindSpecialCards(const std::map<CardRank, int>& card_map,
            CardRank rank) const;

    std::vector<Card> cards;

    Type type;

    CardRank main_rank;

    // for straight-like combinations
    int length;

};

#endif //POKER_CARDCOMBINATION_H
