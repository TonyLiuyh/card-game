//
// Created by apple on 2020/4/25.
//

#include "app/CardCombination.h"
#include <algorithm>

CardCombination::CardCombination() : cards(), type(PASS), main_rank(THREE), length(0) {}

CardCombination::CardCombination(Type type, CardRank main_rank, int length)
    : cards(), type(type), main_rank(main_rank), length(length)
{

}

CardCombination::CardCombination(const std::multiset<Card> &card_set)
    : cards(card_set.begin(), card_set.end()), type(INVALID), main_rank(THREE), length(0) {
    if (card_set.empty()) {
        return;
    }

    std::map<CardRank, std::vector<Card>> card_map;
    for (const Card& card : card_set) {
        if (card_map.find(card.rank) == card_map.end()) {
            card_map.emplace(card.rank, std::vector<Card>());
        }
        card_map[card.rank].push_back(card);
    }
    std::vector<int> rank_numbers(card_map.size());
    int index = 0;
    for (auto & iter : card_map) {
        rank_numbers[index++] = iter.second.size();
    }
    std::sort(rank_numbers.begin(), rank_numbers.end());

    if (card_map.size() == 1) {
        main_rank = cards[0].rank;
        switch (card_map.begin()->second.size()) {
            case 1:
                type = SINGLE;
                break;
            case 2:
                if (cards[0].rank >= BLACK_JOKER) {
                    type = DOUBLE_JOKER;
                } else {
                    type = PAIR;
                }
                break;
            case 3:
                type = THREE_OF_A_KIND;
                break;
            case 4:
                type = FOUR_OF_A_KIND;
                break;
            case 5:
                type = FIVE_OF_A_KIND;
                break;
            case 6:
                type = SIX_OF_A_KIND;
                break;
            case 7:
                type = SEVEN_OF_A_KIND;
                break;
            case 8:
                type = EIGHT_OF_A_KIND;
        }
    } else if (card_map.size() == 2) {
        if (rank_numbers[0] == 1 && rank_numbers[1] == 3) {
            type = THREE_WITH_SINGLE;
            main_rank = cards[1].rank;
        } else if (rank_numbers[0] == 2 && rank_numbers[1] == 3) {
            type = THREE_WITH_PAIR;
            main_rank = cards[2].rank;
        } else if (rank_numbers[0] == 2 && rank_numbers[1] == 4) {
            type = FOUR_WITH_TWO;
            main_rank = cards[2].rank;
        } else if (card_map.begin()->first == BLACK_JOKER) {
            if (card_set.size() == 2) {
                type = DOUBLE_JOKER;
            } else if (card_set.size() == 3) {
                type = TRIPLE_JOKER;
            } else {
                type = QUADRA_JOKER;
            }
        }
    } else if (card_map.size() == 3) {
        if (rank_numbers[0] == 2 && rank_numbers[2] == 2) {
            if (cards[0].rank + 2 == cards[4].rank && cards[4].rank < TWO) {
                type = STRAIGHT_PAIR;
                length = 3;
                main_rank = cards[0].rank;
            }
        } else if (rank_numbers[0] == 1 && rank_numbers[1] == 1 && rank_numbers[2] == 4) {
            type = FOUR_WITH_TWO;
            main_rank = cards[2].rank;
        } else if (rank_numbers[0] == 2 && rank_numbers[1] == 2 && rank_numbers[2] == 4) {
            type = FOUR_WITH_TWO_PAIRS;
            if (cards[0].rank == cards[2].rank) {
                main_rank = cards[0].rank;
            } else if (cards[4].rank == cards[6].rank) {
                main_rank = cards[4].rank;
            } else {
                main_rank = cards[2].rank;
            }
        } else if (rank_numbers[0] == 3 && rank_numbers[2] == 3) {
            if (cards[0].rank + 2 == cards[6].rank && cards[6].rank < TWO) {
                type = STRAIGHT_THREE;
                length = 3;
                main_rank = cards[0].rank;
            }
        } else if (rank_numbers[0] == 2 && rank_numbers[1] == 3 && rank_numbers[1] == 3) {
            type = STRAIGHT_THREE_WITH_SINGLE;
            length = 2;
            bool find_first = false;
            for (auto & iter : card_map) {
                if (iter.second.size() == 3) {
                    if (!find_first) {
                        main_rank = iter.first;
                        find_first = true;
                    } else if (iter.first != main_rank + 1 || iter.first >= TWO) {
                        type = INVALID;
                    }
                }
            }
        }
    } else if (card_map.size() == 4) {
        if (rank_numbers[0] == 2 && rank_numbers[3] == 2) {
            if (cards[0].rank + 3 == cards[6].rank && cards[6].rank < TWO) {
                type = STRAIGHT_PAIR;
                length = 4;
                main_rank = cards[0].rank;
            }
        } else if (rank_numbers[0] == 3 && rank_numbers[3] == 3) {
            if (cards[0].rank + 3 == cards[9].rank && cards[9].rank < TWO) {
                type = STRAIGHT_THREE;
                length = 4;
                main_rank = cards[0].rank;
            }
        } else if (rank_numbers[0] == 1 && rank_numbers[1] == 1 && rank_numbers[2] == 3 &&
                rank_numbers[3] == 3) {
            type = STRAIGHT_THREE_WITH_SINGLE;
            length = 2;
            bool find_first = false;
            for (auto & iter : card_map) {
                if (iter.second.size() == 3) {
                    if (!find_first) {
                        main_rank = iter.first;
                        find_first = true;
                    } else if (iter.first != main_rank + 1 || iter.first >= TWO) {
                        type = INVALID;
                    }
                }
            }
        } else if (rank_numbers[0] == 2 && rank_numbers[1] == 2 && rank_numbers[2] == 3 &&
                   rank_numbers[3] == 3) {
            type = STRAIGHT_THREE_WITH_PAIR;
            length = 2;
            bool find_first = false;
            for (auto & iter : card_map) {
                if (iter.second.size() == 3) {
                    if (!find_first) {
                        main_rank = iter.first;
                        find_first = true;
                    } else if (iter.first != main_rank + 1 || iter.first >= TWO) {
                        type = INVALID;
                    }
                }
            }
        }
    } else if (rank_numbers[0] == rank_numbers[card_map.size() - 1]) {
        if (rank_numbers[0] == 1) {
            type = STRAIGHT;
        } else if (rank_numbers[0] == 2) {
            type = STRAIGHT_PAIR;
        } else if (rank_numbers[0] == 3) {
            type = STRAIGHT_THREE;
        }
        auto iter = card_map.begin();
        int rank = iter->first;
        main_rank = iter->first;
        ++iter;
        for (; iter != card_map.end(); ++iter) {
            if (iter->first != rank + 1 || iter->first >= TWO) {
                type = INVALID;
            }
            ++rank;
        }
        if (type != INVALID) {
            length = rank - main_rank + 1;
        }
    }

    // deal with 3-1 separately
    if (cards.size() % 4 == 0 && card_map.size() > cards.size() / 4) {
        if (rank_numbers[card_map.size() - 1] == rank_numbers[card_map.size() - cards.size() / 4] &&
            rank_numbers[card_map.size() - 1] == 3) {
            type = STRAIGHT_THREE_WITH_SINGLE;
            length = cards.size() / 4;
            bool find_first = false;
            int rank = 0;
            for (auto & iter : card_map) {
                if (iter.second.size() == 3) {
                    if (!find_first) {
                        main_rank = iter.first;
                        rank = iter.first;
                        find_first = true;
                    } else if (iter.first != rank + 1) {
                        type = INVALID;
                    } else {
                        rank += 1;
                    }
                }
            }
        }
    }

    PositionCards();
}

std::vector<Card> CardCombination::Display() const {
    return cards;
}

CardCombination::Type CardCombination::GetType() const {
    return type;
}

CardRank CardCombination::Rank() const {
    return main_rank;
}

std::unordered_map<CardRank, int> CardCombination::GetHint(const std::multiset<Card>& all_cards) const {


    std::map<CardRank, int> card_map;

    for (auto iter = all_cards.rbegin(); iter != all_cards.rend(); ++iter) {
        ++card_map[iter->rank];
    }
    std::map<CardRank, int> copy_map(card_map);


    std::unordered_map<CardRank, int> result;

    if (type == PASS) {
        CardCombination test(all_cards);
        if (test.GetType() != INVALID) {
            return std::unordered_map<CardRank, int>(card_map.begin(), card_map.end());
        }

        for (auto iter = card_map.begin(); iter != card_map.end(); ++iter) {
            int rank = iter->first;
            int cur_length = 1;
            auto iter2 = iter;
            ++iter2;
            for ( ; iter2 != card_map.end(); ++iter2) {
                if (iter2->first == rank + 1 && iter2->second == iter->second && iter2->first < TWO) {
                    ++cur_length;
                    ++rank;
                } else {
                    break;
                }
            }
            if ((iter->second == 1 && cur_length >= 5) || (iter->second == 2 && cur_length >= 3)) {
                for (int i = 0; i < cur_length; ++i) {
                    result.emplace(iter->first, iter->second);
                    ++iter;
                }
                return result;
            } else if (iter->second == 3) {
                for (auto iter3 = card_map.begin();
                    iter3 != card_map.end() && result.size() < cur_length;
                    ++iter3) {
                    if (iter3->second == 1 && iter->first < TWO) {
                        result.emplace(iter3->first, iter3->second);
                    }
                }
                if (result.size() < cur_length) {
                    result.clear();
                    for (auto iter3 = card_map.begin();
                         iter3 != card_map.end() && result.size() < cur_length;
                         ++iter3) {
                        if (iter3->second == 2 && iter->first < TWO) {
                            result.emplace(iter3->first, iter3->second);
                        }
                    }
                    if (result.size() < cur_length) {
                        result.clear();
                    }

                }
                for (int i = 0; i < cur_length; ++i) {
                    result.emplace(iter->first, iter->second);
                    ++iter;
                }
                return result;
            } else if (iter->second < 4) {
                result.emplace(iter->first, iter->second);
                return result;
            }
        }
    }


    if (type > NORMAL) {
        return FindSpecialCards(card_map, main_rank);
    }


    std::vector<int> required_card_num;
    if (type == SINGLE) {
        required_card_num.push_back(1);
    } else if (type == PAIR) {
        required_card_num.push_back(2);
    } else if (type == THREE_OF_A_KIND) {
        required_card_num.push_back(3);
    } else if (type == THREE_WITH_SINGLE) {
        required_card_num.push_back(3);
        required_card_num.push_back(1);
    } else if (type == THREE_WITH_PAIR) {
        required_card_num.push_back(3);
        required_card_num.push_back(2);
    } else if (type == FOUR_WITH_TWO) {
        required_card_num.push_back(4);
        required_card_num.push_back(1);
        required_card_num.push_back(1);
    } else if (type == FOUR_WITH_TWO) {
        required_card_num.push_back(4);
        required_card_num.push_back(2);
        required_card_num.push_back(2);
    }

    if (!required_card_num.empty()) {
        std::pair<CardRank, int> temp = FindLargerCards(card_map, required_card_num[0], main_rank);
        if (temp.second == 0) {
            return FindSpecialCards(copy_map, main_rank);
        }
        result[temp.first] += temp.second;
        for (int i = 1; i < required_card_num.size(); ++i) {
            temp = FindLargerCards(card_map, required_card_num[i], ANY);
            if (temp.second == 0) {
                return FindSpecialCards(copy_map, main_rank);
            }
            result[temp.first] += temp.second;
        }
        return result;
    }

    int straight = 0;
    int tail = 0;
    if (type == STRAIGHT) {
        straight = 1;
    } else if (type == STRAIGHT_PAIR) {
        straight = 2;
    } else if (type == STRAIGHT_THREE) {
        straight = 3;
    } else if (type == STRAIGHT_THREE_WITH_SINGLE) {
        straight = 3;
        tail = 1;
    } else if (type == STRAIGHT_THREE_WITH_PAIR) {
        straight = 3;
        tail = 2;
    }
    int cur_length = 0;
    int rank = 0;
    //int index = 0;
    for (auto iter = card_map.upper_bound(main_rank); iter != card_map.end() && cur_length < length; ++iter) {
        if (iter->first >= TWO) {
            break;
        }

        if (iter->second >= straight) {
            if (rank == 0 || rank == iter->first - 1) {
                rank = iter->first;
                ++cur_length;
            } else {
                rank = iter->first;
                cur_length = 1;
            }
        } else {
            rank = 0;
            cur_length = 0;
        }
    }


    if (cur_length == length) {
        for (int i = 0; i < cur_length; ++i) {
            auto temp = static_cast<CardRank>(rank - i);
            result.emplace(temp, straight);
            card_map.erase(temp);
        }
        if (tail > 0) {
            for (int i = 0; i < cur_length; ++i) {
                auto temp = FindLargerCards(card_map, tail, ANY);
                if (!temp.second) {
                    result.clear();
                    break;
                } else {
                    result.emplace(temp.first, temp.second);
                }
            }
        }
        if (!result.empty()) {
            //main_rank = static_cast<CardRank>(rank - length + 1);
            return result;
        }
    }


    return FindSpecialCards(copy_map, main_rank);
}

bool CardCombination::operator<(const CardCombination &rhs) const {
    if (type == PASS) {
        return rhs.type != INVALID;
    } else if (rhs.type > NORMAL) {
        return type < rhs.type || (type == rhs.type && main_rank < rhs.main_rank);
    } else if (rhs.type != INVALID) {
        return type == rhs.type && main_rank < rhs.main_rank && length == rhs.length;
    } else {
        return false;
    }
}

void CardCombination::PositionCards() {
    if (type == FOUR_WITH_TWO_PAIRS || type == FOUR_WITH_TWO) {
        std::vector<Card> temp;
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            if (iter->rank == main_rank) {
                temp.push_back(*iter);
            }
        }
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            if (iter->rank != main_rank) {
                temp.push_back(*iter);
            }
        }
        std::swap(cards, temp);
    } else if (type == STRAIGHT_THREE_WITH_SINGLE || type == STRAIGHT_THREE_WITH_PAIR) {
        std::vector<Card> temp;
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            if (iter->rank >= main_rank && iter->rank < main_rank + length) {
                temp.push_back(*iter);
            }
        }
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            if (!(iter->rank >= main_rank && iter->rank < main_rank + length)) {
                temp.push_back(*iter);
            }
        }
        std::swap(cards, temp);
    } else if (type != INVALID) {
        std::vector<Card> temp(cards.rbegin(), cards.rend());
        std::swap(cards, temp);
    }
}

std::pair<CardRank, int> CardCombination::FindLargerCards(std::map<CardRank, int>& card_map,
        int num, CardRank minimum_rank) {
    for (auto iter = card_map.upper_bound(minimum_rank); iter != card_map.end(); ++iter) {
        if (iter->second == num) {
            iter->second -= num;
            return {iter->first, num};
        }
    }
    for (auto iter = card_map.upper_bound(minimum_rank); iter != card_map.end(); ++iter) {
        if (iter->second > num && iter->second < 4) {
            iter->second -= num;
            return {iter->first, num};
        }
    }

    return {THREE, 0};
}

bool CardCombination::FindCards(std::map<CardRank, int> &card_map,
        int num, CardRank rank) {
    auto iter = card_map.find(rank);
    return iter != card_map.end() && iter->second >= num;
}

std::unordered_map<CardRank, int> CardCombination::FindSpecialCards(
        const std::map<CardRank, int> &card_map, CardRank rank) const {
    std::unordered_map<CardRank, int> result;
    int joker = 0;
    if (type < NORMAL) {
        joker = 2;
        for (auto & iter : card_map) {
            if (iter.second >= 4) {
                if (result.empty()) {
                    result.emplace(iter.first, iter.second);
                } else if (iter.second < result.begin()->second) {
                    result.clear();
                    result.emplace(iter.first, iter.second);
                }
            }
        }
        /*
        if (!result.empty()) {
            if (result.begin()->second == 4) {
                type = FOUR_OF_A_KIND;
            } else if (result.begin()->second == 5) {
                type = FIVE_OF_A_KIND;
            } else if (result.begin()->second == 6) {
                type = SIX_OF_A_KIND;
            } else if (result.begin()->second == 7) {
                type = SEVEN_OF_A_KIND;
            } else if (result.begin()->second == 8) {
                type = EIGHT_OF_A_KIND;
            }
        }
         */
    } else {
        int number;
        if (type == FOUR_OF_A_KIND) {
            number = 4;
            joker = 2;
        } else if (type == DOUBLE_JOKER || type == FIVE_OF_A_KIND) {
            number = 5;
            joker = 3;
        } else if (type == SIX_OF_A_KIND) {
            number = 6;
            joker = 3;
        } else if (type == TRIPLE_JOKER || type == SEVEN_OF_A_KIND) {
            number = 7;
            joker = 4;
        } else if (type == EIGHT_OF_A_KIND) {
            number = 8;
            joker = 4;
        } else {
            return result;
        }

        for (auto & iter : card_map) {
            if (iter.second > number) {
                if (result.empty()) {
                    result.emplace(iter.first, iter.second);
                } else if (iter.second < result.begin()->second) {
                    result.clear();
                    result.emplace(iter.first, iter.second);
                }
            } else if (iter.second == number && iter.first > rank) {
                result.clear();
                result.emplace(iter.first, iter.second);
                return result;
            }
        }

    }


    if (!result.empty() && result.begin()->second < joker * 2 + 1) {
        return result;
    }

    int black = 0, red = 0;
    if (card_map.find(BLACK_JOKER) != card_map.end()) {
        black = card_map.find(BLACK_JOKER)->second;
    }
    if (card_map.find(RED_JOKER) != card_map.end()) {
        red = card_map.find(RED_JOKER)->second;
    }

    if (black + red >= joker) {
        result.clear();
        if (black >= joker) {
            result.emplace(BLACK_JOKER, joker);
        } else {
            result.emplace(BLACK_JOKER, black);
            result.emplace(RED_JOKER, joker - black);
        }
    }
    return result;
}