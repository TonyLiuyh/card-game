//
// Created by apple on 2020/4/25.
//

#include "app/CardControl.h"
#include "app/Canvas.h"
#include <curses.h>

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

CardControl::CardControl(const std::vector<Card> &cards, int x, int y, int height, int width)
    : UIComponent(x, y, height, width)
    , cards(cards.begin(), cards.end())
    , last_combination()
    , last_hint()
    , selected(false)
    , message() {

}

void CardControl::AddCards(const std::multiset<Card> &new_cards) {
    for (const auto& card : new_cards) {
        cards.emplace(card);
    }
}

bool CardControl::HasCard(CardRank rank, Suit suit) {
    for (const auto& card : cards) {
        if (card.rank == rank && card.suit == suit) {
            return true;
        }
    }
    return false;
}

void CardControl::Play(const std::multiset<Card> &played_cards) {
    for (const auto& card : played_cards) {
        cards.erase(cards.find(card));
    }
    last_combination = CardCombination();
    last_hint = last_combination;
}

void CardControl::SetMessage(const std::string &msg) {
    message = msg;
}

std::multiset<Card> CardControl::AutoPlay() {
    auto hint = last_combination.GetHint(cards);
    std::multiset<Card> played_cards;
    if (!hint.empty()) {
        for (auto iter = cards.rbegin(); iter != cards.rend(); ++iter) {
            if (hint[iter->rank] > 0) {
                --hint[iter->rank];
                played_cards.emplace(*iter);
            }
        }
    }
    return played_cards;
}

bool CardControl::CanPlayCards() const {
    auto hint = last_combination.GetHint(cards);
    return !hint.empty();
}

void CardControl::LastCombination(const CardCombination &combination) {
    if (combination.GetType() == CardCombination::PASS ||
            combination.GetType() == CardCombination::INVALID) {
        return;
    }
    last_combination = combination;
    last_hint = last_combination;
}

bool CardControl::IsLastCombinationPass() const {
    return last_combination.GetType() == CardCombination::PASS;
}

int CardControl::RemainingCardsNumber() const {
    return cards.size();
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

OpponentCardControl::ptr OpponentCardControl::Create(const std::vector<Card> &cards, int x, int y) {
    return ptr(new OpponentCardControl(cards, x, y));
}

OpponentCardControl::OpponentCardControl(const std::vector<Card> &cards, int x, int y)
    : CardControl(cards, x, y, 8, CardsWidth(10))
    , show_cards(false) {
}

void OpponentCardControl::Draw() {
    Clear();
    if (!message.empty()) {
        mvaddstr(x, y - message.length() / 2, message.c_str());
        return;
    }

    std::vector<Card> small_cards(cards.rbegin(), cards.rend());
    // DrawSmallCards(small_cards, x, y, false);
    if (show_cards) {
        DrawSmallCards(small_cards, x, y, 10);
    } else {
        //DrawSmallCards(small_cards, x, y, 10);
        DrawSmallCardsBackWithHeart3(small_cards, x, y, 10);
    }
}

void OpponentCardControl::Reset(const std::vector<Card> &new_cards) {
    cards = std::multiset<Card>(new_cards.begin(), new_cards.end());
    last_combination = CardCombination();
    last_hint = last_combination;
    show_cards = false;
    message = "";
}

void OpponentCardControl::ShowCards() {
    show_cards = true;
}

PlayerCardControl::ptr PlayerCardControl::Create(const std::vector<Card> &cards, int x, int y) {
    return ptr(new PlayerCardControl(cards, x, y));
}

PlayerCardControl::PlayerCardControl(const std::vector<Card> &cards, int x, int y)
    : CardControl(cards, x, y, 9, CardsWidth(20))
    , selected_cards(cards.size(), false)
    , pointer(0)
    , alt_select_flag(0) {
    selectable = true;
    extra_control = HORIZONTAL;
}

void PlayerCardControl::Reset(const std::vector<Card> &new_cards) {
    cards = std::multiset<Card>(new_cards.begin(), new_cards.end());
    last_combination = CardCombination();
    last_hint = last_combination;
    selected_cards = std::vector<bool>(cards.size(), false);
    pointer = 0;
    alt_select_flag = 0;
    message = "";
}

std::multiset<Card> PlayerCardControl::GetSelectedCards() const {
    std::multiset<Card> result;
    int index = 0;
    for (auto card = cards.rbegin(); card != cards.rend(); ++card) {
        if (selected_cards[index++]) {
            result.emplace(*card);
        }
    }
    return result;
}

void PlayerCardControl::EraseSelected() {
    selected_cards = std::vector<bool>(cards.size(), false);
    pointer = 0;
    alt_select_flag = 0;
}

void PlayerCardControl::Hint() {
    auto hint = last_hint.GetHint(cards);
    std::multiset<Card> hint_set;
    if (!hint.empty()) {
        int index = 0;
        for (auto card = cards.rbegin(); card != cards.rend(); ++card) {
            if (hint[card->rank] > 0) {
                --hint[card->rank];
                selected_cards[index] = true;
                hint_set.emplace(*card);
            } else {
                selected_cards[index] = false;
            }
            ++index;
        }
        last_hint = CardCombination(hint_set);
    } else {
        for (auto && selected_card : selected_cards) {
            selected_card = false;
        }
        last_hint = last_combination;
    }

    Draw();
}

void PlayerCardControl::Draw() {
    Clear();
    if (!message.empty()) {
        mvaddstr(x, y - message.length() / 2, message.c_str());
        return;
    }
    int current_width = CardsWidth(cards.size());
    int ul_x = x - height / 2, ul_y = y - current_width / 2;


    int index = 0;
    for (auto card = cards.rbegin(); card != cards.rend(); ++card) {
        if (selected_cards[index]) {
            DrawCardBack(ul_x + 1, ul_y + index * 3, CARD_WIDTH, CARD_HEIGHT);
            DrawCardFace(ul_x + 1, ul_y + index * 3, CARD_HEIGHT, card->rank, card->suit);
        } else {
            DrawCardBack(ul_x + 2, ul_y + index * 3, CARD_WIDTH, CARD_HEIGHT);
            DrawCardFace(ul_x + 2, ul_y + index * 3, CARD_HEIGHT, card->rank, card->suit);
        }
        ++index;
    }
    if (selected && selectable) {
        int pointer_x = ul_x + 1;
        if (selected_cards[pointer]) {
            --pointer_x;
        }
        int pointer_y = ul_y + pointer * 3 + 1;
        mvaddch(pointer_x, pointer_y, ACS_DARROW);
    }
}


void PlayerCardControl::Select() {
    selected = true;
    alt_select_flag = 0;
}

void PlayerCardControl::Unselect() {
    selected = false;
}

void PlayerCardControl::Click() {
    selected_cards[pointer] = !selected_cards[pointer];
}

bool PlayerCardControl::Left() {
    alt_select_flag = 0;
    if (pointer == 0) {
        return false;
    } else {
        --pointer;
        return true;
    }
}

bool PlayerCardControl::Right() {
    alt_select_flag = 0;
    if (pointer == cards.size() - 1) {
        return false;
    } else {
        ++pointer;
        return true;
    }
}

void PlayerCardControl::AltLeft() {
    if (alt_select_flag != 1) {
        Click();
        alt_select_flag = 1;
    } else if (pointer != 0) {
        --pointer;
        Click();
    }
}

void PlayerCardControl::AltRight() {
    if (alt_select_flag != 2) {
        Click();
        alt_select_flag = 2;
    } else if (pointer != cards.size() - 1) {
        ++pointer;
        Click();
    }
}