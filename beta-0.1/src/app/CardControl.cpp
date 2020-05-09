//
// Created by apple on 2020/4/25.
//

#include "app/CardControl.h"
#include "app/Canvas.h"
#include <curses.h>

CardControl::ptr CardControl::Create(const std::multiset<Card> &initial_cards, bool main_player, int x, int y) {
    return CardControl::ptr(new CardControl(initial_cards, main_player, x, y));
}

void CardControl::DrawCards(const std::vector<Card> &cards, int x, int y) {
    int index = 0;
    for (auto card : cards) {
        DrawCardBack(x, y + index * 3, CARD_WIDTH, CARD_HEIGHT);
        DrawCardFace(x, y + index * 3, CARD_HEIGHT, card.rank, card.suit);
        ++index;
    }
}

void CardControl::DrawSmallCards(const std::vector<Card> &cards, int x, int y, int max_cards_per_line) {
    int width;
    int height = (cards.size() - 1) / max_cards_per_line + 1;
    if (height == 1) {
        width = cards.size();
    } else {
        width = max_cards_per_line;
    }
    height = (height - 1) * 3 + SMALL_CARD_HEIGHT + 2;
    width = (width - 1) * 3 + SMALL_CARD_WIDTH + 2;
    x = x - height / 2;
    y = y - width / 2;

    for (int i = 0; i < cards.size(); ++i) {
        int card_x = x + i / max_cards_per_line * 3;
        int card_y = y + i % max_cards_per_line * 3;
        DrawCardBack(card_x, card_y, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT);
        DrawCardFace(card_x, card_y, SMALL_CARD_HEIGHT, cards[i].rank, cards[i].suit);
    }
}

void CardControl::DrawSmallCardsBack(int cards, int x, int y, int max_cards_per_line) {
    int width;
    int height = (cards - 1) / max_cards_per_line + 1;
    if (height == 1) {
        width = cards;
    } else {
        width = max_cards_per_line;
    }
    height = (height - 1) * 3 + SMALL_CARD_HEIGHT + 2;
    width = (width - 1) * 3 + SMALL_CARD_WIDTH + 2;
    x = x - height / 2;
    y = y - width / 2;

    for (int i = 0; i < cards; ++i) {
        int card_x = x + i / max_cards_per_line * 3;
        int card_y = y + i % max_cards_per_line * 3;
        DrawCardBack(card_x, card_y, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT);
    }
}

void CardControl::DrawSmallCardsBackWithHeart3(const std::vector<Card> &cards, int x, int y, int max_cards_per_line) {
    int width;
    int height = (cards.size() - 1) / max_cards_per_line + 1;
    if (height == 1) {
        width = cards.size();
    } else {
        width = max_cards_per_line;
    }
    height = (height - 1) * 3 + SMALL_CARD_HEIGHT + 2;
    width = (width - 1) * 3 + SMALL_CARD_WIDTH + 2;
    x = x - height / 2;
    y = y - width / 2;

    for (int i = 0; i < cards.size(); ++i) {
        int card_x = x + i / max_cards_per_line * 3;
        int card_y = y + i % max_cards_per_line * 3;
        DrawCardBack(card_x, card_y, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT);
        if (cards[i].rank == THREE && cards[i].suit == HEART) {
            DrawCardFace(card_x, card_y, SMALL_CARD_HEIGHT, cards[i].rank, cards[i].suit);
        }
    }
}


int CardControl::CardsWidth(int num_cards) {
    return num_cards * 3 + CARD_WIDTH - 1;
}

CardControl::CardControl(const std::multiset<Card> &initial_cards, bool main_player, int x, int y)
    : UIComponent(x, y, CardsWidth(main_player ? 20 : 10), main_player ? 9 : 8, main_player, VERTICAL)
    , cards()
    , current_card()
    , pointer(0)
    , select_flag(0)
    , selected(false)
    , last_combination()
    , main_player(main_player)
    , show_cards(false) {

    for (const Card& card : initial_cards) {
        cards.emplace(card, false);
    }
    current_card = cards.rbegin();
}

void CardControl::AddCards(const std::multiset<Card> &new_cards) {
    for (const Card& card : new_cards) {
        cards.emplace(card, false);
    }
}

bool CardControl::HasCard(CardRank rank, Suit suit) {
    for (auto & card : cards) {
        if (card.first.rank == rank && card.first.suit == suit) {
            return true;
        }
    }
    return false;
}

CardCombination CardControl::Play() {
    std::multiset<Card> played;

    for (auto & card : cards) {
        if (card.second) {
            played.emplace(card.first);
        }
    }

    if (played.empty()) {
        return CardCombination();
    }

    CardCombination combination(played);

    if (last_combination < combination) {
        for (auto iter = cards.begin(); iter != cards.end(); ) {
            if (iter->second) {
                cards.erase(iter++);
            } else {
                ++iter;
            }
        }

        pointer = 0;
        current_card = cards.rbegin();
        last_combination = CardCombination();
        Draw();
        return combination;
    } else {
        for (auto & card : cards) {
            if (card.second) {
                card.second = false;
            }
        }
        pointer = 0;
        current_card = cards.rbegin();

        Draw();
        return CardCombination(std::multiset<Card>());
    }
}

void CardControl::Pass() {
    for (auto & card : cards) {
        if (card.second) {
            card.second = false;
        }
    }
    pointer = 0;
    current_card = cards.rbegin();
    last_combination = CardCombination();
    if (main_player) {
        Draw();
    }
}

void CardControl::Hint() {
    std::multiset<Card> all_cards;
    for (auto & card : cards) {
        card.second = false;
        all_cards.emplace(card.first);
    }
    auto temp = last_combination.GetHint(all_cards);
    if (!temp.empty()) {
        for (auto & card : cards) {
            if (temp[card.first.rank] > 0) {
                --temp[card.first.rank];
                card.second = true;
            }
        }
    }
    if (main_player) {
        Draw();
    }
}

void CardControl::LastCombination(const CardCombination &combination) {
    if (combination.GetType() == CardCombination::PASS) {
        return;
    }
    last_combination = combination;
}

bool CardControl::IsLastCombinationPass() const {
    return last_combination.GetType() == CardCombination::PASS;
}

int CardControl::RemainingCardsNumber() const {
    return cards.size();
}

void CardControl::ShowCards() {
    show_cards = true;
}

void CardControl::Draw() {
    Clear();
    if (!main_player) {
        std::vector<Card> small_cards;
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            small_cards.push_back(iter->first);
        }
        // DrawSmallCards(small_cards, x, y, false);
        if (show_cards) {
            DrawSmallCards(small_cards, x, y, 10);
        } else {
            //DrawSmallCards(small_cards, x, y, 10);
            DrawSmallCardsBackWithHeart3(small_cards, x, y, 10);
        }
        return;
    }
    //Canvas::Clear(x, y, width, height);

    int current_width = CardsWidth(cards.size());
    int ul_x = x - height / 2, ul_y = y - current_width / 2;


    int index = 0;
    for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
        if (iter->second) {
            DrawCardBack(ul_x + 1, ul_y + index * 3, CARD_WIDTH, CARD_HEIGHT);
            DrawCardFace(ul_x + 1, ul_y + index * 3, CARD_HEIGHT, iter->first.rank, iter->first.suit);
        } else {
            DrawCardBack(ul_x + 2, ul_y + index * 3, CARD_WIDTH, CARD_HEIGHT);
            DrawCardFace(ul_x + 2, ul_y + index * 3, CARD_HEIGHT, iter->first.rank, iter->first.suit);
        }
        ++index;
    }
    if (selected) {
        int pointer_x = ul_x + 1;
        if (current_card->second) {
            --pointer_x;
        }
        int pointer_y = ul_y + pointer * 3 + 1;
        mvaddch(pointer_x, pointer_y, ACS_DARROW);
    }
}

void CardControl::Select() {
    selected = true;
    select_flag = 0;
}

void CardControl::Unselect() {
    selected = false;
}

void CardControl::Click() {
    current_card->second = !current_card->second;
}

bool CardControl::Left() {
    select_flag = 0;
    if (pointer == 0) {
        return false;
    } else {
        --pointer;
        --current_card;
        return true;
    }
}

bool CardControl::Right() {
    select_flag = 0;
    if (pointer == cards.size() - 1) {
        return false;
    } else {
        ++pointer;
        ++current_card;
        return true;
    }
}

void CardControl::AltLeft() {
    if (select_flag != 1) {
        Click();
        select_flag = 1;
    } else if (pointer != 0) {
        --pointer;
        --current_card;
        Click();
    }
}

void CardControl::AltRight() {
    if (select_flag != 2) {
        Click();
        select_flag = 2;
    } else if (pointer != cards.size() - 1) {
        ++pointer;
        ++current_card;
        Click();
    }
}

void CardControl::DrawCardFace(int x, int y, int height, CardRank rank, Suit suit) {
    move(x, y);
    if (rank == BLACK_JOKER || rank == RED_JOKER) {
        if (rank == RED_JOKER) {
            attron(COLOR_PAIR(Canvas::RED));
        }
        const std::string joker = "JOKER";
        for (int i = 0; i < 5 && i < height; ++i) {
            mvaddch(x + 1 + i, y + 1, joker[i]);
        }
        if (rank == RED_JOKER) {
            attroff(COLOR_PAIR(Canvas::RED));
        }
        return;
    }

    if (suit == DIAMOND || suit == HEART) {
        attron(COLOR_PAIR(Canvas::RED));
    }

    move(x + 1, y + 1);
    if (rank < TEN) {
        addch((int)rank + '0');
    } else if (rank == TEN) {
        addstr("10");
    } else if (rank == JACK) {
        addch('J');
    } else if (rank == QUEEN) {
        addch('Q');
    } else if (rank == KING) {
        addch('K');
    } else if (rank == ACE) {
        addch('A');
    } else if (rank == TWO) {
        addch('2');
    }
    move(x + 2, y + 1);
    if (suit == SPADE) {
        addstr("♠");
    } else if (suit == HEART) {
        addstr("♥");
    } else if (suit == DIAMOND) {
        addstr("♦");
    } else {
        addstr("♣");
    }
    if (suit == DIAMOND || suit == HEART) {
        attroff(COLOR_PAIR(Canvas::RED));
    }
}

void CardControl::DrawCardBack(int x, int y, int width, int height) {
    move(x, y);
    addch(ACS_ULCORNER);
    for (int i = 0; i < width; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);

    for (int i = 1; i <= height; ++i) {
        move(x + i, y);
        addch(ACS_VLINE);
        addstr(std::string(width, ' ').c_str());
        addch(ACS_VLINE);
    }

    move(x + height + 1, y);
    addch(ACS_LLCORNER);
    for (int i = 0; i < width; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);
}