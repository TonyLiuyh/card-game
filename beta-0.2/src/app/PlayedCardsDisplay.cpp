//
// Created by apple on 2020/5/6.
//

#include "app/PlayedCardsDisplay.h"
#include "curses.h"

PlayedCardsDisplay::ptr PlayedCardsDisplay::Create(int x, int y, int max_cards_one_line, int lines) {
    return ptr(new PlayedCardsDisplay(x, y, max_cards_one_line, lines));
}

PlayedCardsDisplay::PlayedCardsDisplay(int x, int y, int max_cards_one_line, int lines)
    : UIComponent(x, y, CardControl::SMALL_CARD_HEIGHT + (lines - 1) * 3 + 2,
            CardControl::SMALL_CARD_WIDTH + (max_cards_one_line - 1) * 3 + 2)
    , cards()
    , max_cards_one_line(max_cards_one_line)
    , lines(lines)
    , message() {}

void PlayedCardsDisplay::Draw() {
    Clear();
    if (!cards.empty()) {
        CardControl::DrawSmallCards(cards, x, y, max_cards_one_line);
    } else {
        mvaddstr(x, y - message.length() / 2, message.c_str());
    }
}

void PlayedCardsDisplay::SetCards(const std::vector<Card> &new_cards) {
    cards = new_cards;
}

void PlayedCardsDisplay::Message(const std::string &msg) {
    cards.clear();
    message = msg;
}