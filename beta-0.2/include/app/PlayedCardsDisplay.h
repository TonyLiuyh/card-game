//
// Created by apple on 2020/5/6.
//

#ifndef POKER_PLAYEDCARDSDISPLAY_H
#define POKER_PLAYEDCARDSDISPLAY_H

#include "app/UIComponent.h"
#include <memory>
#include "app/CardControl.h"
#include <string>

class PlayedCardsDisplay : public UIComponent {
public:
    typedef std::shared_ptr<PlayedCardsDisplay> ptr;

    static ptr Create(int x, int y, int max_cards_one_line, int lines);

    void Draw() override;

    void SetCards(const std::vector<Card>& new_cards);

    void Message(const std::string& msg);

private:
    PlayedCardsDisplay(int x, int y, int max_cards_one_line, int lines);

    std::vector<Card> cards;

    int max_cards_one_line;
    int lines;
    std::string message;
};



#endif //POKER_PLAYEDCARDSDISPLAY_H
